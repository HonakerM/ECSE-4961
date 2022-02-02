#ifndef ZSTD_UTIL_H_
#define ZSTD_UTIL_H_


#include "main.h"

/* 
 * Define
 */
#define CHUNK_BYTE_SIZE 16000
//calulated via     size_t max_compressed_size = zstd::ZSTD_compressBound(CHUNK_BYTE_SIZE);
#define MAX_COMPRESSED_SIZE 16118



/*
 * Function Declerations
*/
size_t zstd_compress_chunk(void* const chunk, void* const comressed);





/*
 * ZSTD Specific includes and defintions
 */
 
namespace zstd{
#include <zstd.h>      // presumes zstd library is installed
}



/*! CHECK
 *! Imported from zstd example
 * Check that the condition holds. If it doesn't print a message and die.
 */
#define CHECK(cond, ...)                        \
    do {                                        \
        if (!(cond)) {                          \
            fprintf(stderr,                     \
                    "%s:%d CHECK(%s) failed: ", \
                    __FILE__,                   \
                    __LINE__,                   \
                    #cond);                     \
            fprintf(stderr, "" __VA_ARGS__);    \
            fprintf(stderr, "\n");              \
            exit(1);                            \
        }                                       \
    } while (0)

/*! CHECK_ZSTD
 *! Imported from zstd example
 * Check the zstd error code and die if an error occurred after printing a
 * message.
 */
#define CHECK_ZSTD(fn)                                           \
    do {                                                         \
        size_t const err = (fn);                                 \
        CHECK(!zstd::ZSTD_isError(err), "%s", zstd::ZSTD_getErrorName(err)); \
    } while (0)
#endif 
