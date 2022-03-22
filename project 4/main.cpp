#include "main.h"
#include "worker.h"

#include <chrono>

bool validate_arguments(int mode, int num_threads, std::string input_file, std::string output_file, std::string query){
    if(mode == -1){
        std::cerr<< "Mode must be set. This can be done with either -e, -d, or -c. See bellow for more information" <<std::endl << std::endl; 
        return false;
    }

    if(input_file == ""){
        std::cerr<< "Input file must be set. This can be done with -f. See bellow for more information" <<std::endl << std::endl; 
        return false;
    }

    if((mode == ENCODE || mode == DECODE )&& output_file == ""){
        std::cerr<< "Input file must be set. This can be done with -f. See bellow for more information" <<std::endl << std::endl; 
        return false;
    }
    return true;
} 

void print_help(std::string command_name){
    std::cerr <<"General usage is " << command_name <<"[-e|-d|-q query] -f <source_filename> -o <output_filename> [-t number_of_threads"<<std::endl;
    std::cerr <<"For more advanced usage see the options bellow"<<std::endl;
    std::cerr<<"-t threads | number of threads to encode" <<std::endl;
    std::cerr<<"-e | signifies encode file operation. This takes preceidence"<<std::endl;
    std::cerr<<"-d | decode file"<<std::endl;
    std::cerr<<"-q query | cout how many queries are in file"<<std::endl;
    std::cerr<<"-f filename | source filename for operations"<<std::endl;
    std::cerr<<"-o output_file | output filename for encoding/decoding operationr"<<std::endl;
    std::cerr<<"-s  | silent. Won't output any timing information"<<std::endl;
}


int main(int argc, char ** argv){
    int opt, mode, num_threads;
    bool silent = false;
    std::string input_file = "";
    std::string output_file = "";
    std::string query = "";

    //setup default valeus
    mode = -1;
    num_threads = 1;
    /*
    -t threads | number of threads to encode
    -tm | dynamically get the max aviable amount of threads

    -e | signifies encode file operation. This takes preceidence
    -d | decode file
    -c query | cout how many queries are in file

    -f filename | source filename for operations
    -o output_file | output filename for encoding/decoding operation
    -s | Silent, Will not output any timing information
    */
    while ((opt = getopt(argc, argv, "sedt:f:o:q:")) != -1) {
        switch (opt) {
            case 'e':
                mode = ENCODE;
                break;
            case 'd':
                mode = DECODE;
                break;
            case 'q':
                mode = QUERY;
                query = std::string(optarg);
                break;
            case 's':
                silent = true;
                break;
            case 'f':
                input_file = std::string(optarg);
                break;
            case 'o':
                output_file = std::string(optarg);
                break;

            case 't':
                //if threads is max then reset num_threads
                if(*optarg == 109){
                    num_threads = -1;
                } else {
                    num_threads = atoi(optarg);
                }

                break;

            default: /* '?' */
                print_help(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    //validate arguments
    if(!validate_arguments(mode, num_threads, input_file, output_file, query)){
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    //create worker
    DictionaryWorker worker = DictionaryWorker(num_threads, !silent);


    auto starttime = std::chrono::high_resolution_clock::now();

    long result = 0;
    if(mode == ENCODE){
        result = worker.encode_file(input_file, output_file);
    } else if (mode == DECODE) {
        result = worker.decode_file(input_file, output_file);
    } else if (mode == QUERY) {
        result = worker.query_file(input_file, query);
    }


    auto endtime = std::chrono::high_resolution_clock::now();
    auto total_time = (std::chrono::duration_cast<std::chrono::milliseconds>(endtime-starttime)).count();

    if(mode == QUERY){
        std::cout<<"There were " << result << " occurrences of " <<query << " in the encoded file" << std::endl;
    }

}