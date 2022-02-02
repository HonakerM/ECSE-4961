#include "worker.h"


ZSTDWorker::ZSTDWorker(char worker_id){
    id = worker_id;

    std::thread(&ZSTDWorker::compression_loop, this);
}

void ZSTDWorker::compression_loop(){
    while(1){
        if(src_chunk!=nullptr){
            status=COMPRESSING;
            dst_size = zstd_compress_chunk(src_chunk, dst_chunk);
            status=COMPLETED;
        }
    }
}


bool ZSTDWorker::compress_chunk(void* chunk){
    if(status == IDLE){
        src_chunk = chunk;
        return true;
    }
    return false;
}



size_t ZSTDWorker::get_compressed_chunk(void *dest_ptr){
    dest_ptr = dst_chunk;
    dst_chunk = nullptr;
    src_chunk = nullptr;
    status = IDLE;
    return dst_size;
}