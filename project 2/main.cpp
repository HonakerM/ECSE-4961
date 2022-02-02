#include "main.h"
#include "worker.h"

int main(int argc, char ** argv){

    int* source_buffer = (int*)malloc(CHUNK_BYTE_SIZE);
    for(uint i =0; i < CHUNK_BYTE_SIZE/sizeof(int); i++){
        source_buffer[i] = i;
    }

    void* dest_buffer = malloc(MAX_COMPRESSED_SIZE);
    size_t compressed_size = zstd_compress_chunk(source_buffer, dest_buffer);
    std::cout << CHUNK_BYTE_SIZE << " -> "<< compressed_size << std::endl;

    free(source_buffer);
    free(dest_buffer);

}