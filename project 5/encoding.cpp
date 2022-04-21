#include "fs_context.h"
#include "main.h"
#include "encoding.h"


static inline void set_delim(short& bytes, char delim) {
    if(delim == ' '){
        bytes &= 0x3F;
    } else if(delim == '\n'){
        bytes |= 0x80;
    } else if (delim == '-'){
        bytes |= 0x30;
    }
    
}
std::vector<char> encode_chunck(std::unordered_map<FUSE_ENCODING_KEY_TYPE, FUSE_ENCODING_VALUE_TYPE>* enc_table, std::string input_string, char delim){
    //if input_string is not found
    std::vector<char> output_vector;
    if(enc_table->find(input_string) == enc_table->end()){
        short size_byte = enc_table->size()+input_string.size();
        if(size_byte > 16384){
            throw std::runtime_error("encoding error: string too long");
        }
        set_delim(size_byte, delim);


        
        output_vector.push_back(((char*)(&size_byte))[0]);
        output_vector.push_back(((char*)(&size_byte))[1]);
        output_vector.insert(output_vector.end(), input_string.begin(), input_string.end());
        
        //ou.tput_stream.write(reinterpret_cast<const char*>(&size_byte), sizeof(short));
        //output_stream.write(input_string.c_str(), input_string.size()*sizeof(char));
        return output_vector;
    } else {
        short size_byte = enc_table->at(input_string);
        set_delim(size_byte, delim);


        output_vector.push_back(((char*)(&size_byte))[0]);
        output_vector.push_back(((char*)(&size_byte))[1]);


        return output_vector;
    }
}
