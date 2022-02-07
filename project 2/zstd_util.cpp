#include "zstd_util.h"


size_t zstd_compress_chunk(void* const chunk_buffer, void*  compressed_buffer)
{
    /* Compress.
     * If you are doing many compressions, you may want to reuse the context.
     * See the multiple_simple_compression.c example.
     */
    size_t const compressed_size = zstd::ZSTD_compress(compressed_buffer, MAX_COMPRESSED_SIZE, chunk_buffer, CHUNK_BYTE_SIZE, 1);
    
    CHECK_ZSTD(compressed_size);

    /* success */
    return compressed_size;
}



