
# Matrix Multiplication
The first project of ECSE-4961 was to multiply two matricies together using SIMD instructions. 

## Installation and Execution
### Prerequisites
This project was created using `g++ v10.2.1-6` with the optional dependency of `GNU Make 4.3`. The [Old Dominion University](https://www.cs.odu.edu/~zeil/cs250PreTest/latest/Public/installingACompiler/) has some simple documentation on how to install `g++` depending on your operating system. The matrix multiplication algrothims require cpu's that support the SSE4.1 and AVX processor instructions. The project will not compile without these extensions. 

### Build
This project can be built using the `Makefile` by calling `make build`. Or by directly using `g++` by calling the following function:

```
g++ -Wall -g *.cpp -o main.o -msse4.1 -mavx 
```

To switch between multiplying  `short` and `float` you can either add the `MATRIX_TYPE=short` argument to make like `make build MATRIX_TYPE=short` or by manually definining it in the `g++` as shown in the snippet below:
```
g++ -Wall -g *.cpp -o main.o -msse4.1 -mavx -D MATRIX_TEST_TYPE=short
```

### Running
Once compiled the multiplicatino can be executed by running `./main.o <size_of_matrix>`


## Structure 

### Program Structure
The program is split into 3 core types of functions `main`, `math` and `matrix`. The `main` program file contains the initial main entry-point as well as any shared variables. The `matrix` files contain the templated `matrix` class and all of the required operations. Finally the `math` files contain the actual matrix multiplication algorithms as well as the SIMD intrinsic calls.

### Data Storage
The overall structure for the matrix is the same for both the `float` and `short` data types. The `matrix` class is a templated class located at [matrix.h](https://github.com/HonakerM/ECSE-4961/blob/main/project%201/matrix.h). The actual matrix is stored as a 2d  array stored on the heap with the unique ability to switch between storing the 2d array in column major and row major order  ( more information about the differences can be found [at this great Wikipedia page](https://en.wikipedia.org/wiki/Row-_and_column-major_order) ). This incurs the cost of completely reallocating the 2d array but it can be a great performance boon as discussed in performance results.

### Algorithm Structure
The general matrix multiplication algorithm is the same for all types. The only difference is how the dot product is calculated. For traditional C++ the dot product is calculated via the built in `*` and `+` operators. For float dot product I utilized the AVX instruction `_mm256_dp_ps` to calculate the dot product of 2 vectors of 8 float elements in one instruction. There are not many 16 bit int AVX instructions so I decided to use SSE instructions which still allows the multiplication and addition of 2 vectors of 8 short elements. 


## Performance Results

### Testing Methodology
Each configuration will be tested 5 times on matrices sized 100, 1000, and 10000. The results will then be averaged and compared via graphs. These tests will only measure the time required to actually multiply the matrices any generation or memory clean up is not included. All times are in milliseconds unless otherwise specified. These results were gathered on a `Intel Xeon E3-1220 v5` with `4gb` of memory running `Ubuntu 20.04`.

### SIMD vs Traditional C++
To compile the program with just simd improvements run `make build MATRIX_TYPE=<short/float>`. This is the same as `g++ -Wall -g *.cpp -o main.o -msse4.1 -mavx -D MATRIX_TEST_TYPE=<short/float>`


The main place SIMD can speed up matrix multiplication is in computing the dot product. The AVX simd instruction `_mm256_dp_ps` for floats and `_mm_mullo_epi16/_mm_add_epi16` for shorts allows computing 8 dot product calculations at one time. As the table below shows this change can net as much as a 70% gain in the 10000 sized matrix multiplication for floats. 

#### Float
| Matrix Size | Traditional C++ Time | AVX SIMD Timing |
|-------------|----------------------------|-----------------------|
| 100         | 3.442                      | 0.971                 |
| 1000        | 3055.158                   | 81.7017               |
| 10000       | 53min 25.908sec              |  15min 23.574sec                  |

#### Short
| Matrix Size | Traditional C++ Time | SSE SIMD Time |
|-------------|:--------------------------:|:-------------------:|
| 100         | 2.643                      | 1.270               |
| 1000        | 2288.206                   | 84.4519             |
| 10000       |                            |                     |



### SIMD with Cache Optimization vs SIMD

To compile the SIMD with cach optimization run `make build_witch_cache_optimization`. This is the same as running `g++ -Wall -g *.cpp -o main.o -msse4.1 -mavx -D CACHE_OPTIMIZATION`

As discussed in the `Structure` section this matrix class has the ability to change the ordering. This can be produce great performance benefits when multiplying matrices. The reason being is that when multiplying matrix `AxB` you need to compute the dot-product of the rows of A and the columns of B. In a traditional `Column Major` matrix where each column is a contiguous block of memory fetching 1 full row of matrix `A` would generate a cache miss every new column. Normally this is solved by using a tiled algorithm so you'd only get one section of a row at a time and multiply all cells needing this value. In our implementation you can instead reorder matrix `A` so that the rows are in contiguous blocks of memory to reduce cache missing. While a tiled algorithm would decrease the cache misses even more I decided not to implement it due to time.

Overall the cache improvement is most noticeable the larger the data set. As the `worst case` column in the table shows, even when reordering both `A` and `B` matrices the impact is marginal on sizes greater than a 1000.  

#### Float
| Matrix Size | Traditional C++ Time | SIMD Instructions | Cache Normal Optimized SIMD | Cache Optimized SIMD (worst case) |
|-------------|----------------------|-------------------|-----------------------------|-----------------------------------|
| 100         | 3.442                      | 0.971                 | 0.3274                      | 0.3616                            |
| 1000        | 3055.158                   | 81.7017                 | 155.2826                    | 154.6638                          |
| 10000       | 53min 25.908sec              |  15min 23.574sec  |                             |                                   |

#### Short

When optimizing cache it is most important to look at the cache rate. I utilized [cachegrind](https://valgrind.org/docs/manual/cg-manual.html) which is a module of [valgrind](https://valgrind.org/downloads/?src=www.discoversdk.com) that is a cache and branch-prediction profiler. You can use the following command to use cachegrind `valgrind --tool=cachegrind ./main.o 1000`.  As the table below showcases the cache miss becomes more important the greater the table size.

#### Float
| Matrix Size | Base SIMD Instructions                     | Cache Optimized Instructions              |
|-------------|--------------------------------------------|-------------------------------------------|
| 1000        | 1.7% Miss rate (2.2% read and 0.1% write)  | 0.6% Miss Rate (0.7% read and 0.2% write) |
| 10000       |                                            |                                           |

#### Short

