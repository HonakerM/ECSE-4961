#ifndef _ENCODING_H_
#define _ENCODING_H_

#include "main.h"
#include "fs_context.h"
#include "log.h"
#include <fstream>

#define CHUNK_SIZE 10
// the encoding format is designed by the first two bits being the 
// the following delmiter either 0 for space or 1 for newline or 2 for end of file
// the next 16 bits represent a number. 
// if the number is within the enc_table->size()
// then it must be that location in the enc_table. otherwise
// then the number-enc->size is the bytes of data following it
std::vector<char> encode_chunck(std::unordered_map<FUSE_ENCODING_KEY_TYPE, FUSE_ENCODING_VALUE_TYPE>* enc_table, std::string input_string, char delim);
std::vector<std::pair<std::string, char>> decode_chunk(std::unordered_map<FUSE_ENCODING_VALUE_TYPE,FUSE_ENCODING_KEY_TYPE >* dec_table, std::string& input_data);
int decoded_size(std::unordered_map<FUSE_ENCODING_VALUE_TYPE,FUSE_ENCODING_KEY_TYPE >* dec_table, std::string path);
#endif // _ENCODING_H_