// General Imports
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <chrono>

//matrix imports
#include "matrix.h"
#include "math.h"


template<class MATTRIX_TYPE> void matrix_tester(uint matrix_size){
    
    std::cout << "Started Geneation" << std::endl;
    matrix<MATTRIX_TYPE> matrix_a = matrix<MATTRIX_TYPE>(matrix_size, matrix_size, true);
    matrix<MATTRIX_TYPE> matrix_b = matrix<MATTRIX_TYPE>(matrix_size, matrix_size, true);

    #ifdef DISPLAY_MATRIX
    std::cout << matrix_a <<std::endl;
    std::cout <<matrix_b <<std::endl;
    #endif

    std::cout << "Finished Geneation & Started Traditional Multiplication" << std::endl;

    auto start_traditional_time = std::chrono::high_resolution_clock::now();
    matrix<MATTRIX_TYPE>* output_mult_trad = mult_matrix(&matrix_a, &matrix_b, false);
    auto end_traditional_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "Finished C++ Multiplication" << std::endl;
    std::cout << "Traditional C++ Execution Time: " << (std::chrono::duration_cast<std::chrono::microseconds>(end_traditional_time-start_traditional_time)).count() << "μs" << std::endl;



    auto start_optimized_time = std::chrono::high_resolution_clock::now();
    matrix<MATTRIX_TYPE>* output_mult_optimized = mult_matrix(&matrix_a, &matrix_b, true);
    auto end_optimized_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "Finished Optimized Multiplication" << std::endl;
    std::cout << "Optimized  Execution Time: " << (std::chrono::duration_cast<std::chrono::microseconds>(end_optimized_time-start_optimized_time)).count() << "μs" << std::endl;
    
    #ifdef DISPLAY_MATRIX
    std::cout << *output_mult_trad <<std::endl;
    std::cout << *output_mult_optimized <<std::endl;
    #endif

    //assert(*output_mult_trad == *output_mult_optimized);
}
int main(int argc, char *argv[])
{   
    //set static seed for performance testing
    srand(0);

    /*
    const float in_1[16] = {1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0};
    const float in_2[16] = {1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0};

    float output = simd_dot_product((const float *)&in_1, (const float *)&in_2, 10);
    printf("%f\n",output);
    */


    uint matrix_size;
    if(argc < 2){
        std::cout << "Setting Matrix size to 10. To set custom size run ./main.o [matrix_size]" << std::endl;
        matrix_size = 10;
    } else {
        matrix_size = std::stoi(argv[1]);
    }

    #ifndef MATRIX_TEST_TYPE
        matrix_tester<float>(matrix_size);
    #else
        matrix_tester<MATRIX_TEST_TYPE>(matrix_size);
    #endif


}


