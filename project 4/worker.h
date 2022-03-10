#ifndef WORKER_H_
#define WORKER_H_

//include general model
#include "main.h"

//include specific libraries
#include <pthread.h>
#include <unordered_map>
#include <vector>



//defines requierd by DictionaryWorker
#define TOKEN_TYPE


class DictionaryWorker {
public:
    DictionaryWorker(int num_of_threads);
    ~DictionaryWorker();

    /*
     * Encoding Functions
     */
    //general encode function
    void encode_file(std::string source_file, std::string output_file);

    //encode invidual chunk
    void encode_chunk(std::string file_stream, long start, long count);


    long query_file(std::string encoded_file, std::string search_string);


private:
    int encoding_threads;

    std::unordered_map<std::string, int>* encoding_table;
    unsigned int next_token = 0;
    
};

#endif 