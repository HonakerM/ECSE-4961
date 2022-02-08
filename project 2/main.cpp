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

    worker.compress_chunk(source_buffer, MAX_COMPRESSED_SIZE);
    while(worker.get_compression_status() != COMPLETED ) {
        //std::cout << worker.get_compression_status() << std::endl;
    }

    worker_dest_buffer = malloc(CHUNK_BYTE_SIZE);
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

void compress_large_chunk(uint num_of_workers, void* chunk, uint chunk_size){
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
    size_t max_output_size = zstd::ZSTD_compressBound(chunk_size);
    void* output_buffer = malloc(max_output_size);

    //get size remainging
    uint size_remaining = chunk_size;
    uint worker_section[num_of_workers];
    uint current_slice = 0;

    while(size_remaining!=0){
        for(uint i=0; i< num_of_workers; i++){
            ZSTDWorker* worker = worker_array[i];
            uint status = worker->get_compression_status();
            
            if(status == COMPLETED){
               
            }
            
            if(status == IDLE){
                // 
                std::cout<<"Assigning Block "<< current_slice << " to worker "<< worker->get_id()<< std::endl;

                // assign the current section
                worker_section[i] = current_slice;

                // get compressed size if not enough  data left
                uint compressing_size = CHUNK_BYTE_SIZE;
                if(size_remaining < CHUNK_BYTE_SIZE ){
                    compressing_size = size_remaining;
                }

                // compress chunk
                //worker->compress_chunk(chunk[current_slice*CHUNK_BYTE_SIZE], compressing_size);
                
                // set update size_remaining
                size_remaining = size_remaining - compressing_size;

                //increment current slice
                current_slice++;
            }
        }
    }


    //close all workers
    for(uint i=0; i< num_of_workers; i++){
        ZSTDWorker* worker = worker_array[i];
        worker->exit_loop();
        thread_array[i].join();

        delete worker;
    }   

    free(output_buffer);

}

int main(int argc, char ** argv){
    //individual_test();
    compress_large_chunk(5, nullptr, CHUNK_BYTE_SIZE*6);
}