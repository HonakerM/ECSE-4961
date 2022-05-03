
# Filesystem Dictionary Encoder

  

  

This is the fifth and final project for ECSE-4961. The goal of this project is to create a working realtime dictionary encoding filesystem that will is focused around optimizing SQL backups. This project utilizes the [Filesystem in USErspace (FUSE)](https://www.kernel.org/doc/html/latest/filesystems/fuse.html) kernel interface which was created to allow secure software based filesystem mounts to extend what applications are capable of doing. 
 

## Installation and Execution

  

  

### Prerequisites

  

  

This project was created using `g++ v10.2.1-6` with the optional dependencies of `GNU Make 4.3` and `pkg-config 0.29.1` . The [Old Dominion University](https://www.cs.odu.edu/~zeil/cs250PreTest/latest/Public/installingACompiler/) has some simple documentation on how to install `g++` depending on your operating system. For the threading aspect of the application the [pthread library](https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html) is required.  As mentioned previously this project is based around `FUSE 2.9.9` which can be installed on debian based systems with `apt-get install libfuse-dev` or can be built from the [libfuse github repository](https://github.com/libfuse/libfuse/releases/tag/fuse-2.9.9). 

  

  

### Downloading

  

  

This project can either be downloaded as a zip or cloned using the following git command:

  

```

  

git clone https://github.com/HonakerM/ECSE-4961.git

  

```

  

  

### Build

  

  

This project can be built using the `Makefile` by calling `make`. Or by directly using `g++` with the following options. If you do not have `pkg-config` installed it

  

  

```

  
g++ -Wall main.cpp log.cpp fs.cpp encoding.cpp -I/usr/include/fuse -lfuse -pthread -o main.o -D_FILE_OFFSET_BITS=64

  

```

  

  

### Execution

  

  

Once compiled the filesystem can be started by running the following command where `src_dir` is where the raw filesystem data is stored, `mount_dir` is where the filesystem will be mounted and accessible by the user, and `enc_table` is the encoding table to use for the filesystem.

  

```
./main.o <src_dir> <mount_dir> <enc_table>

```

## Structure

### SQL Encoding
Before talking about the encoding table and data structure it is important to understand the target data format. The postgres backup utility [pg_dump](https://www.postgresql.org/docs/current/app-pgdump.html) exports data in plain-text files containing the SQL commands required to reconstruct the database to the state it was in at the time it was saved. These commands are usually a set of keywords as shown in the example script below:
```
ALTER TABLE call ADD CONSTRAINT call_employee
    FOREIGN KEY (employee_id)
    REFERENCES employee (id);
ALTER TABLE customer ADD CONSTRAINT customer_city
    FOREIGN KEY (city_id)
    REFERENCES city (id);
INSERT  INTO  call_outcome (outcome_text)  VALUES  ('call started');
INSERT  INTO  call_outcome (outcome_text)  VALUES  ('finished - successfully');
INSERT  INTO  call_outcome (outcome_text)  VALUES  ('finished - unsuccessfully');
```
To encode this data the commands are first split at either space and newline characters and then each resultant substring is checked in the encoding table and if a match is found is replaced by the token. 



### Encoding Table Structure

  

The encoding table is stored in an [std::unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map) which is a hash table. For encoding each key is the string of characters with the value being the token while for decoding/querying it is the opposite. The encoding table is stored in a ascii file where each line is a seperate key with the line number being the encoding token. For example in the example file below `key1`'s token would be `1` while `key2` would be `2` and so on.

  

```

key1

key2

key3

```

  

### Encoded Data Structure

The encoded data can be described as a series of individually decodable chunks with each chunk being split into two possible parts: `metadata` and `data`. The metadata is a constant 2 bytes in which the first 2 bits determine what delimiter follows this associated data and the remaining 14 bits represent the encoded data size or location. As for the delimiter you can refer to the following table for what number represents which delimiter.
|Metadata|Delimiter  |
|--|--|
| 00 | space |
| 01 | newline |
| 10 | end of file |
| 11 | reserved |


The remaining 14 bits represent a number `n` that if smaller than the `encoding table size` represents the string stored at `n` in the encoding table. If `n` is greater than the `encoding table size` than there is a set of raw bytes of `n-enc_table_size` size following the short.
```
 0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |Del|Encoded Data Size or Location| Data as needed ....
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   ```

### Encoding Example

Let's encode a simple example using the above encoding table and the following string
```
key1 a 
key2
```
The application reaches its first delimiter at `<space>` (`00`) and checks to see if `key1` is in the encoding table which it is with a token of 1. Thus the first word is encoded as bellow:
```
0000000000000001
```
The next delimiter is `\n` (`01`) with a key of `a` which is not in the encoding table thus the short is encoded to `4` which is calculated from `data_size + encoded_table_size = 1 + 3` . Then `a` is placed after the encoded data thus the full data is
```
0000000000000001010000000000010001100001
*  first  key  *
``` 
Finally the last key is the end of the file thus its encoded to `10`. `key2` is also a match thus the following short is encoded with a token of 2. Thus the completed encoded data is as follows:
```
00000000000000010100000000000100011000011000000000000010
* first string *     second string     *  third string *
```

In this example the encoded data is `56` bits compared to the original data of `96` bits; however, this was a pretty favorable example. As talked about in Results this is not always the case.

## Implementation
### Fuse
The core fuse operation were forked from [the big brother file system](https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/) written by Joseph J. Pfeiffer at New Mexico State University. This is why project 5 is licensed under the [Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License](http://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US).  My main changes were to the `fuse_context` and the `bb_read`/`bb_write`/`bb_getattr` functions

### Fuse Context
The  `FsContext` class was created to hold all of the important data in memory for the file operations. This includes the encoding and decoding table as well as the offset table (more on that in the next section). 

### Fuse Write
The `bb_write` function handles all of the `write()` system calls to the filesystem. This function was modified to to scan the `char buf` for one of the delimiters. Once a delimiter is found the data is encoded and written to the `src_dir`. 

 One major issue this creates is that the number of bytes that are written can sometimes be different then then what a higher level application expects. In the `write` man pages says a smaller returned number means an error while a larger number is undefined. All applications that were tested: `echo`,`mv`,`cp`,`vim` all encountered some form of error. The solution was to keep an offset_table that kept track of the expected bytes written and the actual bytes written. The limitation this creates is that all operations must be sequential from the beginning of a file. This also has the benefit of greatly simplifying the decoding scheme.

### Fuse Read
The `bb_read` is structure similarly to the `bb_write` except that it reads the encoded data key by key extracting the information.  


### Fuse Getattr
This operation had to updated to return the expected number of bytes of the file and not the encoded data's size

## Results

This application was tested with an encoding table based on a [list of the postgres keywords](https://www.postgresql.org/docs/current/sql-keywords-appendix.html) and was benchmarked against  two different data sets: a postgres backup and an excerpt of a Charles Dickinson novel. 

|Name | Raw Data Size (bytes)|Encoded Data Size (bytes)  |
|--|--|--|
|Postgres Backup |1448184|1447985|
| Charles Dickinson Test| 46953|47095  |

As you can see in the above table when using a targeted dataset there is a marginal storage improvement while a random selection, with a large amount of key misses, creates a encoded file that is larger than the original data.

Speed is not talked about in this report as it is really really bad. The performance impact of the bbfs was considerable.

### Interesting Hiccups
- LSB vs MSB
- Reporting size /lieing to the filesystem
