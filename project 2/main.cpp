#include "main.h"
#include "worker.h"
#include "zstd_util.h"
#include <chrono>


//test compressing individual chunks
void individual_test(){
    int* source_buffer = (int*)malloc(CHUNK_BYTE_SIZE);
    for(uint i =0; i < CHUNK_BYTE_SIZE/sizeof(int); i++){
        source_buffer[i] = i;
    }

    #ifdef TEST_COMPRESSION
    void* dest_buffer = malloc(MAX_COMPRESSED_SIZE);

    size_t compressed_size_test = zstd_compress_chunk(source_buffer, MAX_COMPRESSED_SIZE, dest_buffer);
    std::cout << CHUNK_BYTE_SIZE << " -> "<< compressed_size_test << std::endl;
    
    unsigned long long const rSize = zstd::ZSTD_getFrameContentSize(dest_buffer, compressed_size_test);
    void * decompressed_dest_buffer_test = malloc(rSize);

    size_t decompressed_size_test = zstd::ZSTD_decompress(decompressed_dest_buffer_test, rSize, dest_buffer, compressed_size_test);
    CHECK_ZSTD(decompressed_size_test);


    for(uint i; i< decompressed_size_test/sizeof(int); i++){
        if(source_buffer[i]  != ((int*)decompressed_dest_buffer_test)[i]){
            std::cout<<"Decompressed Does not match"<<std::endl;
        }
    }
    return;
    #endif

    void* worker_dest_buffer;

    ZSTDWorker worker = ZSTDWorker('a');
    std::thread worker_1_thread(&ZSTDWorker::compression_loop, &worker);

    worker.compress_chunk(source_buffer, CHUNK_BYTE_SIZE);
    while(worker.get_compression_status() != COMPLETED ) {
        //std::cout << worker.get_compression_status() << std::endl;
    }

    worker_dest_buffer = malloc(MAX_COMPRESSED_SIZE);
    size_t compressed_size = worker.get_compressed_chunk(worker_dest_buffer);
    
    std::cout << CHUNK_BYTE_SIZE << " -> "<< compressed_size << std::endl;


    unsigned long long const rSize = zstd::ZSTD_getFrameContentSize(worker_dest_buffer, compressed_size);

    void * decompressed_dest_buffer = malloc(rSize);
    size_t decompressed_size = zstd::ZSTD_decompress(decompressed_dest_buffer, rSize,worker_dest_buffer, compressed_size);
    CHECK_ZSTD(decompressed_size);
    std::cout<<decompressed_size<<std::endl;

    for(uint i=0; i< decompressed_size/sizeof(int); i++){
        if(source_buffer[i]  != ((int*)decompressed_dest_buffer)[i]){
            std::cout<<"Decompressed does not match"<<std::endl;
            break;
        }
    }

    free(source_buffer);
    free(decompressed_dest_buffer);
    free(worker_dest_buffer);

    worker.exit_loop();
    worker_1_thread.join();
}


// read 16k chunk from a file
long read_chunk(std::ifstream& stream, void* buffer ){
    stream.read((char*)buffer, CHUNK_BYTE_SIZE);
    return stream.gcount();
}

// compress input_file and store it in output_filename
void compress_file(uint num_of_workers, std::string input_filename, std::string output_filename){
    
    auto startup_time = std::chrono::high_resolution_clock::now();
    
    //generate array of workers and threads
    ZSTDWorker* worker_array[num_of_workers];
    std::thread thread_array[num_of_workers];

    //start up workers
    char start_char = 'a';
    for(uint i=0; i<num_of_workers; i++){
        //update the start char
        if(i==26){
            start_char = 'A'-26;
        }
        //create worker
        ZSTDWorker* worker = new ZSTDWorker(start_char+i);
        
        //start worker thread and add worker to worker_array
        thread_array[i] = std::thread(&ZSTDWorker::compression_loop, worker);
        thread_array[i].detach();

        worker_array[i] = worker;
    }   

    
    //generate output buffer
    std::vector<void*> output_buffer;
    std::vector<size_t> output_buffer_size;


    //configure worker sections and current output slice
    uint worker_section[num_of_workers];
    uint current_slice = 0;

    // keep array of what slice the workers are currently working on
    // this will be used to free the memory after compression
    void* worker_src_slice[num_of_workers];


    // open file for reading
    std::ifstream input_file;
    input_file.open (input_filename, std::ios::binary);

    // define refenrce for input chunk and chunk size
    void* chunk=nullptr;
    long chunk_size=LONG_MAX;

    // define variables used in loop evaluation
    volatile bool all_chunks_completed = false;
    bool reached_eof = false;

    #ifdef DEBUG_OUTPUT
    std::cout<<"Starting data compression"<<std::endl;
    #endif 

    auto endup_time = std::chrono::high_resolution_clock::now();

    

    while(!all_chunks_completed){
        // assume all chunks are completed
        all_chunks_completed = true;

        //loop through all workers
        for(uint i=0; i< num_of_workers; i++){
            //get the worker and its compression status
            ZSTDWorker* worker = worker_array[i];
            uint status = worker->get_compression_status();

            //if worker is not idle then it is owkring
            if(status != IDLE){
                all_chunks_completed = false;
            }
            
            if(status == COMPLETED) {

                //get the worker section
                uint slice = worker_section[i];

                #ifdef DEBUG_OUTPUT
                std::cout<<"Block "<< slice << " from worker "<< worker->get_id()<< " has been compressed"<< std::endl;
                #endif

                // generate memory for dest buffer
                void* dest_buffer = malloc(MAX_COMPRESSED_SIZE);
                size_t compressed_size = worker->get_compressed_chunk(dest_buffer);

                //update output buffer
                output_buffer.at(slice) = dest_buffer;
                output_buffer_size.at(slice) = compressed_size;

                //free the src buffer
                free(worker_src_slice[i]);

                // update status so it will immediatly get assigned a new block
                status = worker->get_compression_status();

            
            } 
            
            if(status == IDLE && !reached_eof){
                
                // if chunk is empty
                // this should always happen
                if(chunk == nullptr){

                    //allocate memory for input chunk
                    chunk = malloc(CHUNK_BYTE_SIZE);

                    //read chunk
                    chunk_size =  read_chunk(input_file, chunk);

                    //if chunk size is 0
                    if(chunk_size == 0){
                        reached_eof = true;
                        free(chunk);
                        
                        continue;
                    }
                }

                // if assigning block then all chunks are not completed
                all_chunks_completed = false;


                #ifdef DEBUG_OUTPUT
                std::cout<<"Assigning Block "<< current_slice << " to worker "<< worker->get_id()<< std::endl;
                #endif

                // assign the current section and assign chunk
                worker_section[i] = current_slice;
                worker_src_slice[i] = chunk;

                // compress chunk
                worker->compress_chunk(chunk, chunk_size);
                
                // increment current slice
                current_slice++;

                // add space in the output vectors
                output_buffer.push_back(nullptr);
                output_buffer_size.push_back(0);

                // reset the current chunk
                chunk = nullptr;
            }
        }
        //std::cout<<all_chunks_completed<<std::endl;
    }

    auto endcompression_time = std::chrono::high_resolution_clock::now();

    // close the input file
    input_file.close();

    // open file for writing
    std::ofstream output_file;
    output_file.open (output_filename, std::ios::binary);

    #ifdef DEBUG_OUTPUT
    std::cout<< "Writing Compressed data to file" <<std::endl;
    #endif



    for ( uint i = 0; i < current_slice ; i++ ) {
        // get buffer and buffer size from vector
        void* buffer = output_buffer.at(i);
        uint size = output_buffer_size.at(i);

        // write the  data to the file
        output_file.write((char*)(buffer), size);

        // free the data allocated for the output
        free(buffer);
    }

    auto endoutputtime = std::chrono::high_resolution_clock::now();


    //close the output file
    output_file.close();

    //trigger all workers to close
    for(uint i=0; i< num_of_workers; i++){
        ZSTDWorker* worker = worker_array[i];
        worker->exit_loop();
        delete worker;
    }   

    auto teardown_time = std::chrono::high_resolution_clock::now();


    auto startup = (std::chrono::duration_cast<std::chrono::milliseconds>(endup_time-startup_time)).count();
    auto compression = (std::chrono::duration_cast<std::chrono::milliseconds>(endcompression_time - endup_time)).count();
    auto writing = (std::chrono::duration_cast<std::chrono::milliseconds>(endoutputtime - endcompression_time)).count();
    auto teardown = (std::chrono::duration_cast<std::chrono::milliseconds>(teardown_time - endoutputtime)).count();

    #ifdef TIME_OUTPUT
    std::cout << "startup time: "<< startup << "ms compression time: " << compression <<"ms output time: "<< writing<< "ms teardown time: "<< teardown << "ms"<< std::endl;
    #endif



        
    

}

int main(int argc, char ** argv){
    //used for testing indinvudal function calls
    //individual_test();

    int worker_count = 5;
    if(argc>1){
        worker_count = atoi(argv[1]);
    }

    std::string input_file;
    std::string output_file;
    if(argc == 4){
        input_file = std::string(argv[2]);
        output_file = std::string(argv[3]);
    }  else {
        input_file = "./test.data";
        output_file = "./test.zstd";
    }

    compress_file(worker_count, input_file, output_file);

}