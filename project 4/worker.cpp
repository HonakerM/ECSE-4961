#include "worker.h"


DictionaryWorker::DictionaryWorker(int num_of_threads){
    if(num_of_threads<1){
        encoding_threads=1;
    } 
    this->encoding_threads = num_of_threads;
    encoding_table = new std::unordered_map<std::string, int>();
}

DictionaryWorker::~DictionaryWorker(){
    delete encoding_table;

}
void DictionaryWorker::encode_file(std::string source_file, std::string output_file){
    // !
    // ! TODO get actual file line count
    // !

    long line_count = 1000;
    long lines_per_worker = line_count / encoding_threads;

    std::vector<pthread_t*> thread_list;
    for(int i=0;i<encoding_threads;i++){
        pthread_t* thread;
        pthread_create(thread, NULL, this->encode_chunk, )
        
        encode_chunk(source_file, i*lines_per_worker, lines_per_worker);
    }


}


std::string DictionaryWorker::encode_chunk(std::string source_file, long start, long count){
    // !
    // ! TODO open file and move to start
    // !

    int current_line = 0;
    std::string next_token;
    while(current_line < count){

        // !
        // ! TODO read next line
        // !
        key_string = "blargh";

        //get token
        TOKEN_TYPE token;
        if(encoding_table->count(key_string)){
            token = encoding_table.at(key_string);
        } else {
            //get next token
            token = next_token;
            next_token++;

            //insert token into table
            encoding_table->insert(key_string, token);
        }

        // !
        // ! Write binary token to temp file
        // !


        //increment counter after processing
        current_line++;
    }

    //return temp file name
    return "";

}

