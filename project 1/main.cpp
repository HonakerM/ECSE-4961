// General Imports
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <chrono>

//matrix imports
#include "matrix.h"
#include "math.h"


template<class MATTRIX_TYPE> void matrx_tester(uint matrix_size){
    
    std::cout << "Started Geneation" << std::endl;
    matrix<MATTRIX_TYPE> matrix_a = matrix<MATTRIX_TYPE>(matrix_size, matrix_size, true);
    matrix<MATTRIX_TYPE> matrix_b = matrix<MATTRIX_TYPE>(matrix_size, matrix_size, true);

    #ifdef DISPLAY_MATRIX
    std::cout << matrix_a <<std::endl;
    std::cout <<matrix_b <<std::endl;
    #endif

    std::cout << "Finished Geneation & Started Traditional Multiplication" << std::endl;

    auto start_traditional_time = std::chrono::high_resolution_clock::now();
    matrix<MATTRIX_TYPE>* output_mult = mult_matrix(&matrix_a, &matrix_b, false);
    auto end_traditional_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "Finished C++ Multiplication" << std::endl;
    std::cout << "Traditional C++ Execution Time: " << (std::chrono::duration_cast<std::chrono::microseconds>(end_traditional_time-start_traditional_time)).count() << "μs" << std::endl;
    delete output_mult;



    auto start_optimized_time = std::chrono::high_resolution_clock::now();
    output_mult = mult_matrix(&matrix_a, &matrix_b, true);
    auto end_optimized_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "Finished Optimized Multiplication" << std::endl;
    std::cout << "Optimized  Execution Time: " << (std::chrono::duration_cast<std::chrono::microseconds>(end_optimized_time-start_optimized_time)).count() << "μs" << std::endl;
    
    #ifdef DISPLAY_MATRIX
    std::cout << *output_mult <<std::endl;
    #endif

    delete output_mult;
}
int main(int argc, char *argv[])
{   
    //set static seed for performance testing
    srand(0);

    /*
    const int in_1[16] = {1,2,3,0,0,0,0,0,1,2,3,0,0,0,0,0};
    const int in_2[16] = {1,2,3,0,0,0,0,0,1,2,3,0,0,0,0,0};

    int output = simd_dot_product((const int *)&in_1, (const int *)&in_2, 16);
    printf("%d\n",output);
    */

    uint matrix_size;
    if(argc < 2){
        std::cout << "Setting Matrix size to 10. To set custom size run ./main.o [matrix_size]" << std::endl;
        matrix_size = 10;
    } else {
        matrix_size = std::stoi(argv[1]);
    }

    matrx_tester<int>(matrix_size);
}


