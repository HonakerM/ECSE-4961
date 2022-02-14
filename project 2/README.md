# Parallel ZSTD Compression

This is the second project for ECSE-4961. The goal of this project is to 
utilize multiple threads to compress a large input file using [zstd](http://facebook.github.io/zstd/). The input file is split into 16K byte chunks and distributed to each thread. 

## Installation and Execution


### Prerequisites

This project was created using `g++ v10.2.1-6` with the optional dependency of `GNU Make 4.3`. The [Old Dominion University](https://www.cs.odu.edu/~zeil/cs250PreTest/latest/Public/installingACompiler/) has some simple documentation on how to install `g++` depending on your operating system. This project also requires that `zstd` libraries have been installed. Linux based install instructions can be found at [the zstd repository](https://github.com/facebook/zstd#build-instructions)

### Downloading

This project can either be downloaded as a zip or cloned using the following git command:
```
git clone https://github.com/HonakerM/ECSE-4961.git
```


### Build

This project can be built using the `Makefile` by calling `make time`. Or by directly using `g++` by callign `g++` with the following options:

```
g++ -std=c++11 -g *.cpp -o ./main.o -lzstd -pthread -D TIME_OUTPUT
```

### Execution

Once compiled tthhe compression can be executed by running the following command. 
```
./main.o <num_of_workers> <input_file> <output_file>
```

## Structure

## Performance Results