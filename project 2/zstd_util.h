#ifndef ZSTD_UTIL_H_
#define ZSTD_UTIL_H_

#include <zstd.h>      // presumes zstd library is installed

/* 
 * Define
 */
#define CHUNK_BYTE_SIZE 16000


/*
 * Functional Declerations
*/
static void compress_chunk(void* const chunk, void* const comressed);



#endif 