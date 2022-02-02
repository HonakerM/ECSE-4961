#ifndef WORKER_H_
#define WORKER_H_

//defines
#define IDLE 0x01
#define COMPRESSING 0x02
#define ERROR 0x03
#define COMPLETED 0x04

//include general model
#include "main.h"
#include "zstd_util.h"

//include threading module
#include <thread>


class ZSTDWorker {
public:
    ZSTDWorker(char worker_id);
    ~ZSTDWorker();

    //threading waiting
    void compression_loop();


    //start compression
    bool compress_chunk(void* chunk);

    //get status
    short get_compression_status() { return status; }

    //
    size_t get_compressed_chunk(void *dest_chunk);
    

private:
    char id;
    short status;


    void* src_chunk=nullptr;
    void* dst_chunk=nullptr;
    size_t dst_size;
};

#endif 