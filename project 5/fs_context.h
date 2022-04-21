#ifndef _FS_CONTEXT_H_
#define _FS_CONTEXT_H_

#include "main.h"


/* encoding table types */
#define FUSE_ENCODING_VALUE_TYPE int
#define FUSE_ENCODING_KEY_TYPE std::string

//true offset, os expected offset
typedef std::pair<off_t, off_t> offset_pair;

class FsContext {
public:
    FsContext() {
        enc_table = new std::unordered_map<FUSE_ENCODING_KEY_TYPE, FUSE_ENCODING_VALUE_TYPE>(); 
        dec_table = new std::unordered_map<FUSE_ENCODING_VALUE_TYPE, FUSE_ENCODING_KEY_TYPE>();
        offset_table = new std::unordered_map<int, offset_pair>();
    }

    FILE *logfile;
    char *rootdir;

    //file descriptor + <true offset, last offset>
    std::unordered_map<int, offset_pair>* offset_table;

    std::unordered_map<FUSE_ENCODING_KEY_TYPE, FUSE_ENCODING_VALUE_TYPE>* enc_table;
    std::unordered_map<FUSE_ENCODING_VALUE_TYPE, FUSE_ENCODING_KEY_TYPE>* dec_table;
};


#define BB_DATA ((FsContext*) fuse_get_context()->private_data)


#endif // _FS_CONTEXT_H_