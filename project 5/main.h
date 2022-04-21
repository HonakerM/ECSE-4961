/*
    This code is based off of the big brother file system from 
*/
#ifndef MAIN_H
#define MAIN_H



#include "config.h"
#include "params.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <fstream>
#include <utility>

#ifdef HAVE_SYS_XATTR_H
#include <sys/xattr.h>
#endif

#include "fs_context.h"

#include "log.h"
#include "fs.h"


#endif