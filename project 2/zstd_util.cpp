#include "zstd_util.h"


static void compress_chunk(void* const chunk_buffer, void*  compressed_buffer)
{
    size_t max_compressed_size = ZSTD_compressBound(CHUNK_BYTE_SIZE);
    compressed_buffer = malloc(max_compressed_size);
    //size_t const cBuffSize = ZSTD_compressBound(fSize);
    //void* const cBuff = malloc_orDie(cBuffSize);

    /* Compress.
     * If you are doing many compressions, you may want to reuse the context.
     * See the multiple_simple_compression.c example.
     */
    size_t const compressed_size = ZSTD_compress(chunk_buffer, CHUNK_BYTE_SIZE, compressed_buffer, max_compressed_size, 1);
    CHECK_ZSTD(compressed_size);

    /* success */
    std::cout << CHUNK_BYTE_SIZE<< " -> "<< compressed_size;
}
