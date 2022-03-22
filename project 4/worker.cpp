#include "worker.h"


/*
 * Helper Functions
 */

//adapted from https://stackoverflow.com/questions/843154/fastest-way-to-find-the-number-of-lines-in-a-text-c
//count the number of bytes left in a ifstream
static unsigned long count_bytes(std::ifstream &ifs) {
    std::vector <char> buff( FILE_BUFFER_SIZE );
    unsigned long n = 0;
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

/*
 * Constructor Functions
 */
DictionaryWorker::DictionaryWorker(int num_of_threads){
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

long DictionaryWorker::file_op(int op, std::string source_file, std::string output_file){
    //open the source file
    std::ifstream ifs(source_file);

    long data_start_loc = 0;
    if(op == DECODE){
       data_start_loc = process_hash_stream(ifs);
    }

    //calculate number of bytes and the bytes per worker
    long num_of_bytes = count_bytes(ifs);

    long bytes_per_worker;
    int extra_bytes;

    if(op == ENCODE){
        bytes_per_worker = num_of_bytes / encoding_threads;

        //claculate how many bytes that were chopped off
        extra_bytes = num_of_bytes - (bytes_per_worker*encoding_threads);
    } else if (op == DECODE){
        long estimated_bytes_per_worker = num_of_bytes / encoding_threads;
        bytes_per_worker = estimated_bytes_per_worker - (estimated_bytes_per_worker % 4);
        extra_bytes = (estimated_bytes_per_worker % 4) * encoding_threads;
    }



    //define vectors to keep track of each thread and their output
    std::vector<std::thread*> thread_list;


    std::vector<std::vector<token_type>*> output_token_list;
    std::vector<std::string*> output_string_list;

    std::vector<token_type>* output_token;
    std::string* output_string;
    for(int i=0;i<encoding_threads;i++){

        // create output stream for thread
        if(op == ENCODE){
            std::vector<token_type>* output_token = new std::vector<token_type>();
        } else if ( op == DECODE){
                output_string = new std::string();
        }

        // define the workers starting location
        long start_loc = (extra_bytes+i*bytes_per_worker);
        if(i==0){
            start_loc = 0;
        }


        //define rthe number of bytes per worker
        long worker_bytes = bytes_per_worker;

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
        }

        // add items to the vectors
        thread_list.push_back(thread_obj);
    }

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

    //open output file
    std::ofstream ofs(output_file);

    if(op == ENCODE){
        //Write hashtable into beginning of file
        ofs << generate_hash_stream().rdbuf();
        ofs << FILE_DELIMITER << std::endl;
    }


    int output_size; 
    if(op == ENCODE){
        output_size = output_token_list.size();
    } else {
        output_size = output_string_list.size();
    }  

    //iterate through all of the thread strings
    for(int i=0; i<output_size; i++){
        
        if(op == ENCODE){
            std::vector<token_type>* output_stream = output_token_list.at(i);

            ofs.write(reinterpret_cast<char*>(&output_stream->at(0)), output_stream->size()*sizeof(token_type));

            delete output_stream;
        } else if (op == DECODE){
            //get string_obj
            std::string* string_obj = output_string_list.at(i);

            //combine all of the thread streams
            ofs << *string_obj;

            delete string_obj;
        } else {
            std::string* string_obj = output_string_list.at(i);

        }

    }

    return num_of_bytes;  
}

long DictionaryWorker::encode_file(std::string source_file, std::string output_file){
    //open the source file
    std::ifstream ifs(source_file);

    //calculate number of bytes and the bytes per worker
    long num_of_bytes = count_bytes(ifs);
    long bytes_per_worker = num_of_bytes / encoding_threads;

    //claculate how many bytes that were chopped off
    int extra_bytes = num_of_bytes - (bytes_per_worker*encoding_threads);

    //define vectors to keep track of each thread and their output
    std::vector<std::thread*> thread_list;
    std::vector<std::vector<token_type>*> thread_output_stream;

    for(int i=0;i<encoding_threads;i++){
        // create output stream for thread
        std::vector<token_type>* output_char_stream = new std::vector<token_type>();

        // define the workers starting location
        long start_loc = (extra_bytes+i*bytes_per_worker);
        if(i==0){
            start_loc = 0;
        }


        //define rthe number of bytes per worker
        long worker_bytes = bytes_per_worker;

        //assign the first thread any extra bytes
        if(i==0){
            worker_bytes += extra_bytes;
        }
        
        // create thread
        std::thread* thread_obj = new std::thread(&DictionaryWorker::encode_chunk, this, source_file, start_loc, worker_bytes, output_char_stream); 

        // add items to the vectors
        thread_list.push_back(thread_obj);
        thread_output_stream.push_back(output_char_stream);
    }

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

    //open output file
    std::ofstream ofs(output_file);

    //Write hashtable into beginning of file
    ofs << generate_hash_stream().rdbuf();
    ofs << FILE_DELIMITER << std::endl;

    
    //iterate through all of the thread strings
    for(int i=0; i<thread_output_stream.size(); i++){
        //get string pointer
        std::vector<token_type>* output_stream = thread_output_stream.at(i);

        ofs.write(reinterpret_cast<char*>(&output_stream->at(0)), output_stream->size()*sizeof(token_type));

        delete output_stream;
    }

    return num_of_bytes;    

}


void DictionaryWorker::encode_chunk(std::string source_file, long start, long count, std::vector<token_type>* output_stream){
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

    //start current ilne
    long bytes_read = 0;
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

/*
 * Decoding Functions
 */

long DictionaryWorker::decode_file(std::string source_file, std::string output_file){
    //open source file
    std::ifstream ifs( source_file );

    //read hash table from ifstream
    long data_start_loc = process_hash_stream(ifs);
    
    //count the following bytes
    long num_of_bytes = count_bytes(ifs);

    //align the bytes_per_worker to be divisble by 4
    //any extra bytes will be assigned to the first worker
    long estimated_bytes_per_worker = num_of_bytes / encoding_threads;
    int extra_bytes = (estimated_bytes_per_worker % 4) * encoding_threads;
    long bytes_per_worker = estimated_bytes_per_worker - (estimated_bytes_per_worker % 4);


    std::vector<std::thread*> thread_list;
    std::vector<std::string*> thread_output_string;
    for(int i=0;i<encoding_threads;i++){

        // create output stream for thread
        std::string* output_string = new std::string();

        // define the workers starting location
        long start_loc = (data_start_loc+extra_bytes+i*bytes_per_worker);
        if(i==0){
            start_loc = data_start_loc;
        }

        //define rthe number of bytes per worker
        long worker_bytes = bytes_per_worker;

        //assign the first thread any extra bytes
        if(i==0){
            worker_bytes += extra_bytes;
        }

        //create the thread
        std::thread* thread_obj = new std::thread(&DictionaryWorker::decode_chunk, this, source_file, start_loc, worker_bytes, output_string); 

        // add items to the vectors
        thread_list.push_back(thread_obj);
        thread_output_string.push_back(output_string);
    }

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

    //open output file
    std::ofstream ofs(output_file);
    
    //iterate through all of the thread strings
    for(int i=0; i<thread_output_string.size(); i++){
        //get string pointer
        std::string* string_obj = thread_output_string.at(i);

        //combine all of the thread streams
        ofs << *string_obj;

        //free string memory
        delete string_obj;

    }

    return num_of_bytes;    
}


void DictionaryWorker::decode_chunk(std::string source_file, long start, long count, std::string* output_string){
    //open file 
    std::ifstream ifs( source_file );

    //seek to the character before our so we can see 
    ifs.seekg(start);

    //start current ilne
    long bytes_read = 0;
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

/*
 * Query functions
*/


long DictionaryWorker::query_file(std::string source_file, std::string search_string){

    //open source file
    std::ifstream ifs( source_file );

    //read hash table from ifstream
    long data_start_loc = process_hash_stream(ifs);
    
    //count the following bytes
    long num_of_bytes = count_bytes(ifs);

    //align the bytes_per_worker to be divisble by 4
    //any extra bytes will be assigned to the first worker
    long estimated_bytes_per_worker = num_of_bytes / encoding_threads;
    int extra_bytes = (estimated_bytes_per_worker % 4) * encoding_threads;
    long bytes_per_worker = estimated_bytes_per_worker - (estimated_bytes_per_worker % 4);


    std::vector<std::thread*> thread_list;
    std::vector<long*> thread_output;
    for(int i=0;i<encoding_threads;i++){

        // create output stream for thread
        long* output_long = new long;

        // define the workers starting location
        long start_loc = (data_start_loc+extra_bytes+i*bytes_per_worker);
        if(i==0){
            start_loc = data_start_loc;
        }

        //define rthe number of bytes per worker
        long worker_bytes = bytes_per_worker;

        //assign the first thread any extra bytes
        if(i==0){
            worker_bytes += extra_bytes;
        }

        //create the thread
        std::thread* thread_obj = new std::thread(&DictionaryWorker::query_chunk, this, source_file, search_string, start_loc, worker_bytes, output_long); 

        // add items to the vectors
        thread_list.push_back(thread_obj);
        thread_output.push_back(output_long);
    }

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

    //open output file
    long output = 0;

    //iterate through all of the thread strings
    for(int i=0; i<thread_output.size(); i++){
        //get string pointer
        long* long_ptr = thread_output.at(i);

        output += *long_ptr;

        //free string memory
        delete long_ptr;

    }

    return output;
    
}


void DictionaryWorker::query_chunk(std::string source_file, std::string search_string, long start, long count, long* output){

    //open file 
    std::ifstream ifs( source_file );

    //seek to the character before our so we can see 
    ifs.seekg(start);

    //start current ilne
    long bytes_read = 0;
    std::string key_string;
    long matches = 0;
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

long DictionaryWorker::process_hash_stream(std::ifstream& stream){


    std::string key_string;    
    char byte;
    long byte_count = 0;
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