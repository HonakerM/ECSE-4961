#include "main.h"
#include "worker.h"
#include "zstd_util.h"

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


long read_chunk(std::ifstream& stream, void* buffer ){
    stream.read((char*)buffer, CHUNK_BYTE_SIZE);
    return stream.gcount();
}

void compress_file(uint num_of_workers, std::string filename){
    ZSTDWorker* worker_array[num_of_workers];
    std::thread thread_array[num_of_workers];

    //start up workers
    for(uint i=0; i<num_of_workers; i++){
        //
        ZSTDWorker* worker = new ZSTDWorker('a'+i);
        
        //start worker thread and add worker to worker_array
        thread_array[i] = std::thread(&ZSTDWorker::compression_loop, worker);
        worker_array[i] = worker;
    }   

    
    //generate output buffer
    std::vector<void*> output_buffer;
    std::vector<int> output_buffer_size;


    //configure worker sections and current output slice
    uint worker_section[num_of_workers];
    uint current_slice = 0;

    // keep array of what slice the workers are currently working on
    // this will be used to free the memory after compression
    void* worker_src_slice[num_of_workers];


    //open file for reading
    std::ifstream input_file;
    input_file.open (filename);

    void* chunk=nullptr;
    long chunk_size=LONG_MAX;
    std::cout<<1<<std::endl;
    while(chunk_size!=0){
        for(uint i=0; i< num_of_workers; i++){
            ZSTDWorker* worker = worker_array[i];
            uint status = worker->get_compression_status();
            
            if(status == COMPLETED){
                //get the worker section
                uint slice = worker_section[i];

                std::cout<<"Block "<< slice << " from worker "<< worker->get_id()<< " has been compressed"<< std::endl;


                // 
                void* dest_buffer = malloc(MAX_COMPRESSED_SIZE);
                size_t compressed_size = worker->get_compressed_chunk(dest_buffer);

                //update output buffer
                output_buffer.at(slice) = dest_buffer;
                output_buffer_size.at(slice) = compressed_size;


                // update status so it will immediatly get assigned a new block
                status = worker->get_compression_status();
            }
            
            if(status == IDLE){

                if(chunk == nullptr){
                    //read in chunk
                    chunk = malloc(CHUNK_BYTE_SIZE);

                    chunk_size =  read_chunk(input_file, chunk);

                    //if chunk size is 0
                    if(chunk_size == 0){
                        break;
                    }
                }

                // 
                std::cout<<"Assigning Block "<< current_slice << " to worker "<< worker->get_id()<< std::endl;


                // assign the current section and assign chunk
                worker_section[i] = current_slice;
                worker_src_slice[i] = chunk;

                // compress chunk
                worker->compress_chunk(chunk, chunk_size);
                
                //increment current slice
                current_slice++;

                //add space in the output vectors
                output_buffer.push_back(nullptr);
                output_buffer_size.push_back(0);

                //reset the current chunk
                chunk = nullptr;
            }
        }
    }


    input_file.close();

    //close all workers
    for(uint i=0; i< num_of_workers; i++){
        ZSTDWorker* worker = worker_array[i];
        worker->exit_loop();
        thread_array[i].join();

        delete worker;
    }   

}

int main(int argc, char ** argv){

    //individual_test();
    compress_file(5, "./test.data");
}