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

The encoded data can be described as a series of individually decodable chunks with each chunk being split into two possible parts: `metadata` and `data`. The metadata is a constant 2 bytes in which the first 2 bits determine what delimiter follows this associated data. You can refer to the following table for what number represents which delimiter.
|Metadata|Delimiter  |
|--|--|
| 00 | space |
| 01 | newline |
| 10 | end of file |
| 11 | reserved |


The remaining 14 bits represent a number `n` that if smaller than the `encoding table size` represents the string stored at `n` in the encoding table. If `n` is greater than the `encoding table size` than there is a set of raw bytes of `n-enc_table_size` size following the short.



Let's encode a simple example using the above encoding table and the following string
```
key1 a 
key2
```
The application reaches its first delimiter at `<space>` (`00`) and checks to see if `key1` is in the encoding table which it is with a token of 1. Thus the first word is encoded into `0001` . The next delimiter is `\n` with the 


### Interesting Hiccups
- LSB vs MSB
- Reporting size /lieing to the filesystem
- 