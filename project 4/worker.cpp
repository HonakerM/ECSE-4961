#include "worker.h"


/*
 * Helper Functions
 */
//adapted from https://stackoverflow.com/questions/843154/fastest-way-to-find-the-number-of-lines-in-a-text-c
static unsigned long read_stream_into_buffer( std::istream & is, std::vector <char> & buff ) {
    is.read( &buff[0], buff.size() );
    return is.gcount();
}

static unsigned long count_new_line_characters( const std::vector <char> & buff) {
    int newlines = 0;
    const char * p = &buff[0];
    for ( int i = 0; i < buff.size(); i++ ) {
        if ( p[i] == '\n' ) {
            newlines++;
        }
    }
    return newlines;
}

static unsigned long count_lines(std::string file_name) {
    std::vector <char> buff( FILE_BUFFER_SIZE );
    std::ifstream ifs( file_name );
    unsigned long n = 0;
    int buffer_count = 0;

    while(true){
        buffer_count = read_stream_into_buffer(ifs, buff);
        if(buffer_count == 0){
            break;
        }


        n += count_new_line_characters(buff);
    }

    return n;
}


static unsigned long count_bytes(std::string file_name) {
    std::vector <char> buff( FILE_BUFFER_SIZE );
    std::ifstream ifs( file_name );
    unsigned long n = 0;
    int buffer_count = 0;

    while(true){
        buffer_count = read_stream_into_buffer(ifs, buff);
        
        if(buffer_count == 0){
            break;
        }

        n += buffer_count;

    }

    return n;
}

/*
 * Dictionary Workers
 */
DictionaryWorker::DictionaryWorker(int num_of_threads){
    if(num_of_threads<1){
        encoding_threads=1;
    } 
    this->encoding_threads = num_of_threads;
    encoding_table = new hash_table_type();
}

DictionaryWorker::~DictionaryWorker(){
    delete encoding_table;

}
void DictionaryWorker::encode_file(std::string source_file, std::string output_file){
    //get line count
    long num_of_bytes = count_bytes(source_file);
    long bytes_per_worker = num_of_bytes / encoding_threads;

//    std::vector<pthread_t*> thread_list;
//    for(int i=0;i<encoding_threads;i++){
//        //get thread object in list
//        pthread_t* thread;
//        thread_list.push_back(thread);

        //encode file
//        std::stringstream output_stream = encode_chunk(source_file, i*bytes_per_worker, bytes_per_worker);
//
//    }
    std::stringstream thread_stream = encode_chunk(source_file, 0, bytes_per_worker);

    // !
    // ! Wait for threads to complete
    // ! 

    // !
    // ! Write hashtable into beginning of file
    // !
    std::stringstream output_stream = generate_hash_stream();
    output_stream << thread_stream.rdbuf();

    // !
    // ! Combine all temp files into one
    // !
    std::ofstream ofs(output_file);
    ofs << output_stream.rdbuf();
}


std::stringstream DictionaryWorker::encode_chunk(std::string source_file, long start, long count){
    //open file 
    std::ifstream ifs( source_file );

    //if not the first worker
    if(start != 0){

        //seek to the character before our so we can see 
       ifs.seekg(start-1);
        
       //skip to the next full line
       while(ifs.get() != '\n');
    } 

    //start current ilne
    long bytes_read = 0;
    std::string key_string;
    std::stringstream output_string;
    token_type token;

    while(bytes_read < count && std::getline(ifs, key_string)){
        //get number of bytes read
        bytes_read += key_string.length();

        // ! MUTEX HASH TABLE
        if(encoding_table->count(key_string)){
            token = encoding_table->at(key_string);
        } else {
            // get next token
            token = next_token;
            next_token++;

            // insert token into table
            encoding_table->insert(std::make_pair(key_string, token));
        }
        // ! DEMUTEX HASH TABLE


        //conevrt the token into char data
        char buffer[sizeof(token_type)];
        for(int i=0; i<sizeof(token_type); i++){
            buffer[i] = (token >> (8*i)) & 0xff;
        };

        //write the raw token data to the stream
        output_string.write(buffer, sizeof(token_type));
    }

    //return temp file name
    return output_string;

}




std::stringstream DictionaryWorker::generate_hash_stream() {
    std::vector<hash_table_pair> hash_list;
    for(hash_table_type::iterator it = encoding_table->begin(); it != encoding_table->end(); it++){
        hash_list.push_back(hash_table_pair(it->second, it->first));
    }

    std::sort(hash_list.begin(), hash_list.end());

    std::stringstream output_stream;

    for(std::vector<hash_table_pair>::iterator it = hash_list.begin(); it != hash_list.end(); it++){
        output_stream << it->second << "\n";
    }

    return output_stream;

}