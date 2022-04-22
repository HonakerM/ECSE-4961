#include "fs_context.h"
#include "main.h"
#include "encoding.h"
#include "log.h"


static inline void set_delim(short& bytes, char delim) {
    if(delim == ' '){
        bytes &= 0x3F;
    } else if(delim == '\n'){
        bytes &= 0x3F;
        bytes |= 0x40;
    } else if (delim == '-'){
        bytes &= 0x3F;
        bytes |= 0x80;
    }
}

static inline char get_delim(short& bytes) {
    if((bytes & 0xC0) == 0x00){
        return ' '; 
    } else if((bytes & 0xC0) == 0x40){
        return '\n';
    } else if ((bytes & 0xC0) == 0x80){
        return '-';
    }
    return '\0';
}

int decoded_size(std::unordered_map<FUSE_ENCODING_VALUE_TYPE,FUSE_ENCODING_KEY_TYPE >* dec_table, std::string path){
    std::string file_content;
    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    file_content = buffer.str();
    log_msg("lstat decoding size is %s\n", file_content.c_str());

    std::vector<std::pair<std::string, char>> returned_vector = decode_chunk(dec_table, file_content);

    int size = 0;
    for(int i = 0; i < returned_vector.size(); i++){
        size += returned_vector[i].first.size();
        if(returned_vector[i].second != '-'){
            size++;
        }
    }
    return size;
}

//returns the number of bytes decoded
std::vector<std::pair<std::string, char>> decode_chunk(std::unordered_map<FUSE_ENCODING_VALUE_TYPE,FUSE_ENCODING_KEY_TYPE >* dec_table, std::string& input_data){
    //if the inital meta data is not avaiable
    //(metadata contains 2 bytes)    
    if(input_data.size() < 2){
        //not enough data to read
        log_msg("\nNot enough data for decoding\n");
        return std::vector<std::pair<std::string, char>>();
    }

    std::vector<std::pair<std::string, char>> output_data;
    while(!input_data.empty()){
        //get the meta data
        short meta_data = (((short)input_data[0]) << 8) | (0x00ff & input_data[1]);
        log_msg("\n Meta data has %x and %x\n",input_data[0], input_data[1]);

        //get delimeter
        char delim = get_delim(meta_data);


        //clear the delimereter meta data to get the actual encoded number
        meta_data &= 0x3F;
        log_msg("\n Meta data is %d with delim %c\n",meta_data, delim);


        //if the meta data is not in the dec_table
        std::string decoded_data;
        if(meta_data >= dec_table->size()){
            int expected_size = meta_data - dec_table->size();
            log_msg("\n Expecting raw data of size %d\n",expected_size);
            if(expected_size > input_data.size()){
                log_msg("\n Not enough data for decoding there is %d left over\n",input_data.size());
                break;
            }

            int num_bytes = meta_data - dec_table->size();
            decoded_data = input_data.substr(2, num_bytes);
            input_data = input_data.substr(num_bytes + 2);

        } else {
            log_msg("\n Key found!\n");

            decoded_data = dec_table->at(meta_data);
            input_data = input_data.substr(2);
        }
        output_data.push_back(std::make_pair(decoded_data, delim));
    }

    return output_data;



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

        log_msg("Encoding short is %x\n", size_byte);

        
        output_vector.push_back(((char*)(&size_byte))[1]);
        output_vector.push_back(((char*)(&size_byte))[0]);
        output_vector.insert(output_vector.end(), input_string.begin(), input_string.end());
        
        //ou.tput_stream.write(reinterpret_cast<const char*>(&size_byte), sizeof(short));
        //output_stream.write(input_string.c_str(), input_string.size()*sizeof(char));
        log_msg("Vector size is %d\n", output_vector.size());

        return output_vector;
    } else {
        short size_byte = enc_table->at(input_string);
        set_delim(size_byte, delim);

        log_msg("Encoding short is %x\n", size_byte);

        output_vector.push_back(((char*)(&size_byte))[1]);
        output_vector.push_back(((char*)(&size_byte))[0]);

        log_msg("Vector size is %d\n", output_vector.size());
        return output_vector;
    }


}
