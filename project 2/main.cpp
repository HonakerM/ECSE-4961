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

    size_t compressed_size_test = zstd_compress_chunk(source_buffer, dest_buffer);
    std::cout << CHUNK_BYTE_SIZE << " -> "<< compressed_size << std::endl;
    
    void * decompressed_dest_buffer = malloc(CHUNK_BYTE_SIZE);
    size_t decompressed_size = zstd::ZSTD_decompress(decompressed_dest_buffer, CHUNK_BYTE_SIZE, dest_buffer, compressed_size_test);
    for(uint i; i< decompressed_size/sizeof(int); i++){
        if(source_buffer[i]  != ((int*)decompressed_dest_buffer)[i]){
            std::cout<<"Decompressed Does not match"<<std::endl;
        }
    }
    return 0;
    #endif

    void* worker_dest_buffer;

    ZSTDWorker worker = ZSTDWorker('a');
    std::thread worker_1_thread(&ZSTDWorker::compression_loop, &worker);

    worker.compress_chunk(source_buffer);
    while(worker.get_compression_status() != COMPLETED ){
        //std::cout<< worker.get_compression_status() <<std::endl;
    }
    worker_dest_buffer = malloc(CHUNK_BYTE_SIZE);
    size_t compressed_size = worker.get_compressed_chunk(worker_dest_buffer);
    
    std::cout << CHUNK_BYTE_SIZE << " -> "<< compressed_size << std::endl;


    void * decompressed_dest_buffer = malloc(CHUNK_BYTE_SIZE);
    size_t decompressed_size = zstd::ZSTD_decompress(decompressed_dest_buffer, CHUNK_BYTE_SIZE,worker_dest_buffer, compressed_size);
    std::cout<<decompressed_size<<std::endl;

    for(uint i=0; i< decompressed_size/sizeof(int); i++){
        if(source_buffer[i]  != ((int*)decompressed_dest_buffer)[i]){
            std::cout<<"Decompressed Does not match"<<std::endl;
        }
    }

    free(source_buffer);
    free(decompressed_dest_buffer);
    free(worker_dest_buffer);

    worker.exit_loop();
    worker_1_thread.join();
}

void compress_large_chunk(uint num_of_workers, void* chunk){
    ZSTDWorker* worker_array[num_of_workers];
    std::thread thread_array[num_of_workers];

    //start up workers
    for(uint i=0; i<num_of_workers; i++){
        //
        ZSTDWorker* worker = new ZSTDWorker('a'+i);
        
        //
        thread_array[i] = std::thread(&ZSTDWorker::compression_loop, worker);
        worker_array[i] = worker;
    }   


    //close all workers
    for(uint i=0; i< num_of_workers; i++){
        ZSTDWorker* worker = worker_array[i];
        (*worker).exit_loop();
        thread_array[i].join();

        delete worker;
    }   


}

int main(int argc, char ** argv){
    //individual_test();
    compress_large_chunk(5, nullptr);
}