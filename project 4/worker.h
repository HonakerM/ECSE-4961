#ifndef WORKER_H_
#define WORKER_H_

//include general model
#include "main.h"

//include specific libraries

//threading libraries
#include <thread>
#include <mutex>

//include for hast ables
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <sstream>

//types and defines requierd by DictionaryWorker
typedef unsigned int token_type;
typedef std::pair<token_type, std::string> hash_table_pair;
typedef std::unordered_map<std::string, token_type> encode_table_type;
typedef std::unordered_map<token_type, std::string> decode_table_type;

#define FILE_BUFFER_SIZE 1024 * 1024
#define FILE_DELIMITER "|||||||||||||||"

union CharTokenUnion{
    //the char array will be resized to match the token_type
    char char_data[sizeof(token_type)];
    token_type raw_data;
};


class DictionaryWorker {
public:
    DictionaryWorker(int num_of_threads);
    DictionaryWorker(const DictionaryWorker&);
    ~DictionaryWorker();

    //encode functions
    void encode_file(std::string source_file, std::string output_file);

    //decode functions
    void decode_file(std::string encoded_file, std::string decoded_file);

    //query functions
    long query_file(std::string encoded_file, std::string search_string);



private:
    //private helper functions used to perform the actual encoding/decoding
    void encode_chunk(std::string source_file, long start, long count, std::vector<token_type>* output_stream);
    long query_chunk(std::string encoded_file, std::string search_string, long start, long count);
    void decode_chunk(std::string file_stream, long start, long count, std::string* output_stream);

    long calculate_worst_case_encoding(long num_of_bytes){return (num_of_bytes/2)*sizeof(token_type);}


    //hash table functions
    std::stringstream generate_hash_stream();
    long process_hash_stream(std::ifstream& stream);


    int encoding_threads;


    //only the encoding table needs a mutex
    //as read operations are atomic for decoding
    std::mutex encoding_mutex;
    
    encode_table_type* encoding_table;
    decode_table_type* decoding_table;


    token_type next_token = 0;
    
};

#endif 