#include "worker.h"


ZSTDWorker::ZSTDWorker(char worker_id){
    id = worker_id;
    status = IDLE;
}


ZSTDWorker::~ZSTDWorker(){
    exit_loop();
}

void ZSTDWorker::compression_loop(){
    while(1){
        if(status == EXITED) {
            std::cout<<"Exiting worker "<<id<<std::endl;
            return;
        }
        if(status==WAITING_FOR_COMPRESSION && src_chunk!=nullptr){
            status=COMPRESSING;

            //allocate space for destination buffer
            dst_chunk = malloc(MAX_COMPRESSED_SIZE);

            //compress chunk
            dst_size = zstd_compress_chunk(src_chunk, dst_chunk);

            //mark compression as complete
            status=COMPLETED;
        }
    }
}

void ZSTDWorker::exit_loop(){
    status = EXITED;
}


bool ZSTDWorker::compress_chunk(void* chunk){
    if(status == IDLE){
        dst_size = 0;
        src_chunk = chunk;
        status = WAITING_FOR_COMPRESSION;
        return true;
    }
    return false;
}



size_t ZSTDWorker::get_compressed_chunk(void* dest_ptr){
    
    
    std::memcpy(dest_ptr, dst_chunk, dst_size);
    free(dst_chunk);
    src_chunk = nullptr;
    status = IDLE;
    return dst_size;
}