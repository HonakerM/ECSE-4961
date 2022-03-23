#include "worker.h"


/*
 * Helper Functions
 */

//adapted from https://stackoverflow.com/questions/843154/fastest-way-to-find-the-number-of-lines-in-a-text-c
//count the number of bytes left in a ifstream
static unsigned int count_bytes(std::ifstream &ifs) {
    std::vector <char> buff( FILE_BUFFER_SIZE );
    unsigned int n = 0;
    int buffer_count = 0;

    while(true){
        ifs.read( &buff[0], buff.size() );
        buffer_count = ifs.gcount();
        
        if(buffer_count == 0){
            break;
        }

        n += buffer_count;
    }

    return n;
}

extern "C" {
    
    //
    static unsigned int compute_simd_sum(int* array, int size){

        int mem_size = size;
        if(size % SIMD_BATCH_SIZE != 0){
            mem_size += size % SIMD_BATCH_SIZE;
        }

        int* buffered_array = (int*)malloc(mem_size);
        memcpy(buffered_array, array, size*sizeof(int));

        //
        for(int i=0; i < mem_size-size; i++){
            buffered_array[size + i] = 0;
        }


        // define output
        __m128i sum = _mm_setzero_si128();

        for(uint i=0; i < size; i += SIMD_BATCH_SIZE){
            __m128i reg;

            //load values into registers
            reg = _mm_loadu_si128 ( (__m128i*)&buffered_array[i] );

            //add the dotproduct to the sum accumulator
            sum = _mm_add_epi32 ( sum , reg );
        }

        //
        int* returned_vector = (int*)malloc(SIMD_BATCH_SIZE*sizeof(int));
        _mm_store_si128((__m128i*)&returned_vector, sum);


        //compute the verticle sum of the final vector
        int total_sum = 0;
        for(int i=0; i < SIMD_BATCH_SIZE; i++){
            total_sum += returned_vector[i];
        }

        //return the vertical sum
        return total_sum;
    }
}

/*
 * Constructor Functions
 */
DictionaryWorker::DictionaryWorker(int num_of_threads, bool silent){
    this->report_timing = silent;

    //if num_of_threads is less 1 then set it to the idle number of threads
    if(num_of_threads<1){
        encoding_threads=std::thread::hardware_concurrency();

        //if the hardware_concurrency call is disabled then default to 0
        if(encoding_threads<1){
            encoding_threads = 1;
        }

        std::cout << "Setting the number of workers to "<< encoding_threads <<std::endl;
    } else {
        encoding_threads = num_of_threads;
    }

    encoding_table = new encode_table_type();
    decoding_table = new decode_table_type();

}

//custom copy is required by std::mutex
DictionaryWorker::DictionaryWorker(const DictionaryWorker& worker){
    this->encoding_threads = worker.encoding_threads;
    encoding_table = worker.encoding_table;
    decoding_table = worker.decoding_table;

}

DictionaryWorker::~DictionaryWorker(){
    delete encoding_table;
    delete decoding_table;
}

int DictionaryWorker::file_op(int op, std::string source_file, std::string output_file){
    auto starttime = std::chrono::high_resolution_clock::now();

    //open the source file
    std::ifstream ifs(source_file);

    //confirm that the stream was opened correctly
    if(!ifs.is_open()){
        std::cerr<<"The file: "<<source_file <<" could not be opened"<<std::endl;
        exit(1);
    }


    //process any required metadata
    int data_start_loc = 0;
    if(op == DECODE || op == QUERY){
       data_start_loc = process_hash_stream(ifs);
    }

    //calculate number of bytes of data to process 
    int num_of_bytes = count_bytes(ifs);

    //calcualte the bytes per worker and the number of extra bytes
    int bytes_per_worker;
    int extra_bytes;
    if(op == ENCODE){
        //encoding offset is handled by the encode_chunk
        bytes_per_worker = num_of_bytes / encoding_threads;

        //claculate how many bytes that were chopped off by rounding
        extra_bytes = num_of_bytes - (bytes_per_worker*encoding_threads);
    } else if (op == DECODE || op == QUERY){
        //estimate the number of bytes per worker
        int estimated_bytes_per_worker = num_of_bytes / encoding_threads;

        //align each worker by 4 bytes for decoding
        bytes_per_worker = estimated_bytes_per_worker - (estimated_bytes_per_worker % 4);
        extra_bytes = (estimated_bytes_per_worker % 4) * encoding_threads;

        //if alignment isn't possible with this thread count then exit
        if(bytes_per_worker%4 != 0 || extra_bytes%4 != 0){
            std::cerr << "Invalid thread count. exiting" <<std::endl;
            exit(1);
        }
    }

    auto inital_processing = std::chrono::high_resolution_clock::now();

    //define vectors to keep track of each thread and their output
    std::vector<std::thread*> thread_list;



    //allocate a vector for all possible options
    //this is primarily for compiler warnings due to void *
    //the overhead is marginable
    std::vector<std::vector<token_type>*> output_token_list;
    std::vector<std::string*> output_string_list;
    int* int_list = (int*)calloc(encoding_threads,sizeof(int));
    int int_size = encoding_threads;

    //similar situation as above
    std::vector<token_type>* output_token;
    std::string* output_string;
    int* output_int;
    std::string search_string = "";

    for(int i=0;i<encoding_threads;i++){

        // create output stream for thread
        if(op == ENCODE){
            output_token = new std::vector<token_type>();
        } else if ( op == DECODE){
            output_string = new std::string();
        } else if( op == QUERY){
            output_int = &int_list[i];
        }

        // define the workers starting location
        int start_loc;
        
        if(op == ENCODE){
            start_loc = (extra_bytes+i*bytes_per_worker);
        } else if (op == DECODE || op == QUERY){
            start_loc = (data_start_loc+extra_bytes+i*bytes_per_worker);
        }
         

        if(i==0){
            start_loc = data_start_loc;
        }

        //define rthe number of bytes per worker
        int worker_bytes = bytes_per_worker;

        //assign the first thread any extra bytes
        if(i==0){
            worker_bytes += extra_bytes;
        }
        
        // create thread
        std::thread* thread_obj;
        if(op == ENCODE){
            thread_obj = new std::thread(&DictionaryWorker::encode_chunk, this, source_file, start_loc, worker_bytes, output_token); 
        
            output_token_list.push_back(output_token);
        } else if (op == DECODE) {
            thread_obj = new std::thread(&DictionaryWorker::decode_chunk, this, source_file, start_loc, worker_bytes, output_string); 

            output_string_list.push_back(output_string);
        } else if (op == QUERY) {

            thread_obj = new std::thread(&DictionaryWorker::query_chunk, this, source_file, output_file, start_loc, worker_bytes,output_int); 

        }

        // add items to the vectors
        thread_list.push_back(thread_obj);
    }

    auto thread_distribution = std::chrono::high_resolution_clock::now();


    //while some threads are still running
    while(!thread_list.empty()){
        
        //iterate through list of running threads
        for(int i=0; i < thread_list.size(); i++){
            //get threading boject
            std::thread* thread_obj = thread_list.at(i);

            //check if thread is joinable
            if(thread_obj->joinable()){
                //delete the thread
                thread_obj->join();

                //remove it from the thread list
                thread_list.erase(thread_list.begin() + i);       

                //free thread obj memory
                delete thread_obj;         
            }

        }
    }

    auto encoding_finish = std::chrono::high_resolution_clock::now();

    int return_number = 0;

    //query does not require writing to a file so its handled elsewhere
    if(op != QUERY){

        //open output file
        std::ofstream ofs(output_file);

        //if encoded then output the hash
        if(op == ENCODE){
            //Write hashtable into beginning of file
            ofs << generate_hash_stream().rdbuf();
            ofs << FILE_DELIMITER << std::endl;
        }

        
        //iterate through all of the thread strings
        for(int i=0; i<encoding_threads; i++){
            
            if(op == ENCODE){
                std::vector<token_type>* token_ptr = output_token_list.at(i);

                ofs.write(reinterpret_cast<char*>(&token_ptr->at(0)), token_ptr->size()*sizeof(token_type));

                delete token_ptr;
            } else if (op == DECODE){
                //get string_obj
                std::string* string_ptr = output_string_list.at(i);

                //combine all of the thread streams
                ofs << *string_ptr;

                delete string_ptr;
            }
        }

        return_number = num_of_bytes;

    } else {
        //I implemented this addition in simd only to realize
        // that int_size is just encoding_threads so there is no
        //benifit to using sse


        //return_number = compute_simd_sum(int_list, int_size);
        for(int i=0; i<int_size; i++){
            int int_ptr = int_list[i];

            return_number += int_ptr;
        }
        delete(int_list);
    }


    //calculate timing information
    auto finish_reading = std::chrono::high_resolution_clock::now();
    auto inital_processing_time = (std::chrono::duration_cast<std::chrono::milliseconds>(inital_processing-starttime)).count();
    auto distribute_time = (std::chrono::duration_cast<std::chrono::milliseconds>(thread_distribution-inital_processing)).count();
    auto encoding_time = (std::chrono::duration_cast<std::chrono::milliseconds>(encoding_finish-thread_distribution)).count();
    auto writing_time = (std::chrono::duration_cast<std::chrono::milliseconds>(finish_reading-encoding_finish)).count();

    if(report_timing){
        std::cout<<"inital_proicessing: "<<inital_processing_time<<"ms thread_startup_time:"<<distribute_time<<"ms encoding_time:"<<encoding_time<<"ms writing_time:"<<writing_time<<"ms"<<std::endl;
    }

    //return the number
    return return_number;
}

int DictionaryWorker::encode_file(std::string source_file, std::string output_file){
    return file_op(ENCODE, source_file, output_file);
}

int DictionaryWorker::decode_file(std::string source_file, std::string output_file){
    return file_op(DECODE, source_file, output_file);
}

int DictionaryWorker::query_file(std::string source_file, std::string search_string){
    return file_op(QUERY, source_file, search_string);
}

/*
 * Chunk Operations
 */

void DictionaryWorker::encode_chunk(std::string source_file, int start, int count, std::vector<token_type>* output_stream){
    //open file 
    std::ifstream ifs( source_file );

    //if not the first worker
    if(start != 0){

        //seek to the character before our so we can see 
       ifs.seekg(start-1);
    
       //skip to the next full line
       while(ifs.get() != '\n'){
           //move the start location
           start++;

           //decrement count each time so no line is double read
           count--;
       }
    } 

    //helper variables 
    int bytes_read = 0;
    std::string key_string;
    token_type token;


    while(bytes_read < count && std::getline(ifs, key_string)){
        // get number of bytes read 
        // add 1 due to the new line character
        bytes_read += key_string.length()+1;

        //if key is the file delimiter then skip
        if(key_string == FILE_DELIMITER){
            std::cerr << "Invalid key: "<< FILE_DELIMITER <<" is a reserved key. Skipping line" <<std::endl;
            continue;
        }

        //take ownership of the encoding table
        encoding_mutex.lock();

        //if hit then get the preassigned token
        if(encoding_table->count(key_string)){
            token = encoding_table->at(key_string);
        } else {
            // get next token
            token = next_token;
            next_token++;

            // insert token into table
            encoding_table->insert(std::make_pair(key_string, token));
        }

        //relese mutex
        encoding_mutex.unlock();

        //add value to output_stream
        output_stream->push_back(token);
    }

}


void DictionaryWorker::decode_chunk(std::string source_file, int start, int count, std::string* output_string){
    //open file 
    std::ifstream ifs( source_file );

    //seek to the character before our so we can see 
    ifs.seekg(start);

    //start current ilne
    int bytes_read = 0;
    std::string key_string;
    char token_buffer; 
    token_type token;
    while(bytes_read < count){
        //reset token
        token = 0;

        //read token
        ifs.read(reinterpret_cast<char*>(&token), sizeof(token_type));
        bytes_read += sizeof(token_type);

        //find the token in the decoding table
        decode_table_type::iterator key_it = decoding_table->find(token);

        //if key is not found then skip
        if(key_it == decoding_table->end()){
            std::cerr << "Thread: "<< std::this_thread::get_id()<<" : Token "<< token <<" not found in table. Skipping token" <<std::endl;
            continue;
        }
        
        //get the string value
        key_string = key_it->second;
        
        //write the raw token data to the stream
        (*output_string) += key_string + "\n";
    }
}

void DictionaryWorker::query_chunk(std::string source_file, std::string search_string, int start, int count, int* output){

    //open file 
    std::ifstream ifs( source_file );

    //seek to the character before our so we can see 
    ifs.seekg(start);

    //loop trackers
    int bytes_read = 0;
    int matches = 0;

    //temp variables
    token_type token;
    std::string key_string;


    while(bytes_read < count){
        //reset token
        token = 0;

        //read token
        ifs.read(reinterpret_cast<char*>(&token), sizeof(token_type));
        bytes_read += sizeof(token_type);

        //find the token in the decoding table
        decode_table_type::iterator key_it = decoding_table->find(token);

        //if key is not found then skip
        if(key_it == decoding_table->end()){
            std::cerr << "Thread: "<< std::this_thread::get_id()<<" : Token "<< token <<" not found in table. Skipping token" <<std::endl;
            continue;
        }
        
        //get the string value
        key_string = key_it->second;
        
        //increment matches
        if(key_string == search_string){
            matches++;
        }        
    }

    //copy the result to the output
    *output = matches;
}

/*
 * Hash table Encoding/Decoding Functions
 */

std::stringstream DictionaryWorker::generate_hash_stream() {
    //get list of items in the format <token, key>
    std::vector<hash_table_pair> hash_list;
    for(encode_table_type::iterator it = encoding_table->begin(); it != encoding_table->end(); it++){
        hash_list.push_back(hash_table_pair(it->second, it->first));
    }

    //sort the hash by keys
    std::sort(hash_list.begin(), hash_list.end());


    //output each key on new line
    std::stringstream output_stream;
    for(std::vector<hash_table_pair>::iterator it = hash_list.begin(); it != hash_list.end(); it++){
        output_stream << it->second << "\n";
    }

    return output_stream;

}

int DictionaryWorker::process_hash_stream(std::ifstream& stream){


    std::string key_string;    
    char byte;
    int byte_count = 0;
    while(stream.get(byte)){
        //increment the byte count
        byte_count++;

        //if byte if a new line character then the key is completed
        if(byte == '\n'){
            //if file delimeter then return
            if(key_string == FILE_DELIMITER){
                return byte_count;
            }

            //insert key into hash table
            decoding_table->insert(std::make_pair(next_token, key_string));

            //increment next_token
            next_token++;

            //reset key_string
            key_string = "";
        } else {
           key_string.push_back(byte);
        }
        
    }
    
    return byte_count;


}