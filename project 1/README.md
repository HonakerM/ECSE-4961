# Matrix Multiplication
The first project of ECSE-4961 was to multiply two matricies together using SIMD instructions. 

## Installation and Execution
### Prerequisites
This project was created using `g++ v10.2.1-6` with the optional dependency of `GNU Make 4.3`. The [Old Dominion University](https://www.cs.odu.edu/~zeil/cs250PreTest/latest/Public/installingACompiler/) has some simple documentation on how to install `g++` depending on your operating system. This project also utilizes the SSE4.1 and AVX processor instructions. This project will not compile without these extensions. 

### Build
This project can be built using the `Makefile` by calling `make build`. Or by directly using `g++` by calling the following function:

```
g++ -Wall -g *.cpp -o main.o -msse4.1 -mavx
```

To switch between multiplying  `short` and `float` you can either add the `MATRIX_TYPE=short` argument to make like `make build MATRIX_TYPE=short` or by manually definining it in the `g++` as shown in the snippet below:
```
g++ -Wall -g *.cpp -o main.o -msse4.1 -mavx -D CACHE_OPTIMIZATION -D MATRIX_TEST_TYPE=short
```

### Running
Once compiled the multiplicatino can be executed by running `./main.o <size_of_matrix>`


## Structure 

### Program Structure
The program is split into 3 core types of functions `main`, `math` and `matrix`. The `main` program file contains the inital main entrypoint as well as any shared variables. The `matrix` files contain the templated `matrix` class and all of the required operations. Finally the `math` files contain the actual matrix multiplication algorithms as well as the SIMD intrinsic calls.

### Data Storage
The overall structure for the matrix is the same for both the `float` and `short` data types. The `matrix` class is a temlpated class located at [matrix.h](https://github.com/HonakerM/ECSE-4961/blob/main/project%201/matrix.h). The actual matrix is stored as a 2d  array stored on the heap. The matrix class has the unique ability to switch between storing the 2d array in column major and row major order ( more information about the differnces can be found [at this great wikipedia page](https://en.wikipedia.org/wiki/Row-_and_column-major_order) ). This incures the cost of completely reallocating the 2d array but it can be a great performance boon. 


## Performance Results

### Testing Methology
Each configuration will be tested 5 times on matricies sized 100, 1000, and 10000. The results will then be averaged and compared via graphs. These tests will only measure the time required to actually multiply the matrices any generation or memory clean up is not included. Times are in milliseconds. 

### SIMD vs Traditional C++
To compile the program with just simd improvements run `make build`. This is the same as `g++ -Wall -g *.cpp -o main.o -msse4.1 -mavx`


The main place SIMD can speed up matrix multiplication is in computing the dot product. The AVX simd instruction `_mm256_dp_ps` or the `short` equivilant allows computing 8 dot product calculations in one instruction. As the table below shows this change can net as much as a % gain in the 10000 sized matrix multiplication for floats. 

#### Float
| Matrix Size | Traditional C++ Time | AVX SIMD Timing |
|-------------|----------------------------|-----------------------|
| 100         | 3.442                      | 0.971                 |
| 1000        | 3055.158                   | 81.7017               |
| 10000       |                            |                       |

#### Short
| Matrix Size | Traditional C++ Time | AVX SIMD Time |
|-------------|:--------------------------:|:-------------------:|
| 100         | 2.643                      | 1.270               |
| 1000        | 2288.206                   | 84.4519             |
| 10000       |                            |                     |



### SIMD with Cache Optimization vs SIMD

To compile the SIMD with cach optimization run `make build_witch_cache_optimization`. This is the same as running `g++ -Wall -g *.cpp -o main.o -msse4.1 -mavx -D CACHE_OPTIMIZATION`

As discussed in the `Structure` section this matrix class has the ability to change the ordering. This can be produce greate performance benifits when multiplying matricies. The reason being is that when multiplying matrix `AxB` you need to compute the dotproduct of the rows of A and the columns of B. In a traditional `Column Major` matrix where each column is a contigous block of memory fetching 1 full row of matrix `A` would generate a cache miss every new column. Normally this is solved by using a tiled algorithm so you'd only get one section of a row at a time and multiply all cells needing this value. In our implementation you can instead reorder matrix `A` so that the rows are in contigous blocks of memory to reduce cache missing. 

Overall the cache improvement is most noticable the larger the data set. As the `worst case` column in the table shows, even when the multiplication requiresing reordering both `A` and `B` matricies the impact is marginal on larger matricies.  

#### Float
| Matrix Size | Traditional C++ Time | SIMD Instructions | Cache Normal Optimized SIMD | Cache Optimized SIMD (worst case) |
|-------------|----------------------|-------------------|-----------------------------|-----------------------------------|
| 100         | 3.442                | 0.4154            | 0.3274                      | 0.3616                            |
| 1000        | 3055.158             | 174.1992          | 155.2826                    | 154.6638                          |
| 10000       |                      |                   |                             |                                   |

#### Short

When optimizing cache it is most important to look at the cache rate. I utilized [cachegrind](https://valgrind.org/docs/manual/cg-manual.html) which is a module of [valgrind](https://valgrind.org/downloads/?src=www.discoversdk.com) that will profile a cache and branch-prediction profiler. You can use the following command to use cachegrind `valgrind --tool=cachegrind ./main.o 1000`.  As the table below showcases the cache miss becomes more important the greater the table size.

#### Float
| Matrix Size | Base SIMD Instructions                     | Cache Optimized Instructions              |
|-------------|--------------------------------------------|-------------------------------------------|
| 1000        | 1.7% Miss rate (2.2% read and 0.1% write)  | 0.6% Miss Rate (0.7% read and 0.2% write) |
| 10000       |                                            |                                           |


### C++ Optimization

For fun testing we decided to test how well `g++` will optimize the code. We ran the same build but with the `-Ofast` option which will make `g++` spend more time optimizing calls to try to speed up the application. As the below table shows this
