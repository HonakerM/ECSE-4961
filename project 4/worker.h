#ifndef WORKER_H_
#define WORKER_H_

//include general model
#include "main.h"

//include specific libraries

//threading libraries
#include <pthread.h>

//include for hast ables
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <sstream>

//types and defines requierd by DictionaryWorker
typedef unsigned int token_type;
typedef std::pair<token_type, std::string> hash_table_pair;
typedef std::unordered_map<std::string, token_type> hash_table_type;

#define FILE_BUFFER_SIZE 1024 * 1024

class DictionaryWorker {
public:
    DictionaryWorker(int num_of_threads);
    ~DictionaryWorker();

    //encode functions
    void encode_file(std::string source_file, std::string output_file);
    std::stringstream encode_chunk(std::string source_file, long start, long count);

    //decode functions
    void decode_file(std::string encoded_file, std::string decoded_file);
    std::string decode_chunk(std::string file_stream, long start, long count);

    //query functions
    long query_file(std::string encoded_file, std::string search_string);
    long query_chunk(std::string encoded_file, std::string search_string, long start, long count);

    //
    std::stringstream generate_hash_stream();
    void process_hash_stream(std::stringstream stream);

private:
    int encoding_threads;

    hash_table_type* encoding_table;
    token_type next_token = 0;
    
};

#endif 