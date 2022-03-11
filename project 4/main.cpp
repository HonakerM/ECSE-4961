#include "main.h"

#include "worker.h"



#define ENCODE 1
#define DECODE 2
#define QUERY 3


void print_help(std::string command_name){
    std::cerr <<"General usage is " << command_name <<"[-e|-d|-c] -f <source_filename> -o <output_filename> [-t number_of_threads"<<std::endl;
    std::cerr <<"For more advanced usage see the options bellow"<<std::endl;
    std::cerr<<"-t threads | number of threads to encode" <<std::endl;
    std::cerr<<"-e | signifies encode file operation. This takes preceidence"<<std::endl;
    std::cerr<<"-d | decode file"<<std::endl;
    std::cerr<<"-c query | cout how many queries are in file"<<std::endl;
    std::cerr<<"-f filename | source filename for operations"<<std::endl;
    std::cerr<<"-o output_file | output filename for encoding/decoding operationr"<<std::endl;

}
int main(int argc, char ** argv){
    int opt, mode, num_threads;
    std::string input_file = "";
    std::string output_file = "";
    std::string query = "";

    //setup default valeus
    mode = -1;
    num_threads = 1;
    /*
    -t threads | number of threads to encode

    -e | signifies encode file operation. This takes preceidence
    -d | decode file
    -c query | cout how many queries are in file

    -f filename | source filename for operations
    -o output_file | output filename for encoding/decoding operationr
    */
    while ((opt = getopt(argc, argv, "edt:f:o:c:")) != -1) {
        switch (opt) {
            case 'e':
                mode = ENCODE;
                break;
            case 'd':
                mode = DECODE;
                break;
            case 'c':
                mode = QUERY;
                query = std::string(optarg);
                break;
            
            case 'f':
                input_file = std::string(optarg);
                break;
            case 'o':
                output_file = std::string(optarg);
                break;

            case 't':
                num_threads = atoi(optarg);
                break;

            default: /* '?' */
                print_help(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if(mode == -1){
        std::cerr<< "Mode must be set. This can be done with either -e, -d, or -c. See bellow for more information" <<std::endl << std::endl; 
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    if(input_file == ""){
        std::cerr<< "Input file must be set. This can be done with -f. See bellow for more information" <<std::endl << std::endl; 
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    if(mode == ENCODE && output_file == ""){
        std::cerr<< "Input file must be set. This can be done with -f. See bellow for more information" <<std::endl << std::endl; 
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    std::cout<< mode <<std::endl;
    std::cout<< input_file <<"|" <<output_file <<"|"<<query <<std::endl;
    std::cout<< num_threads <<std::endl;



    //create worker
    DictionaryWorker worker = DictionaryWorker(num_threads);

    if(mode == ENCODE){
        worker.encode_file(input_file, output_file);
    }
}