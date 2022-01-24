// General Imports
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <chrono>

//matrix imports
#include "matrix.h"
#include "math.h"


int main(int argc, char *argv[])
{   
    //set static seed for performance testing
    srand(0);

    /*
    const float in_1[16] = {1,2,3,0,0,0,0,0,1,2,3,0,0,0,0,0};
    const float in_2[16] = {1,2,3,0,0,0,0,0,1,2,3,0,0,0,0,0};

    float output = fl_dot_product((const float *)&in_1, (const float *)&in_2, 16);
    printf("%f\n",output);
    */

    uint matrix_size;
    if(argc < 2){
        std::cout << "Setting Matrix size to 10. To set custom size run ./main.o [matrix_size]" << std::endl;
        matrix_size = 10;
    } else {
        matrix_size = std::stoi(argv[1]);
    }


    std::cout << "Started Geneation" << std::endl;
    
    matrix<float> matrix_a = matrix<float>(matrix_size, matrix_size, true);
    matrix<float> matrix_b = matrix<float>(matrix_size, matrix_size, true);

    std::cout << "Finished Geneation & Started Traditional Multiplication" << std::endl;

    auto start_traditional_time = std::chrono::high_resolution_clock::now();
    matrix<float>* output_mult = fl_mult_matrix(&matrix_a, &matrix_b, fl_sisd_dot_product);
    auto end_traditional_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "Finished C++ Multiplication" << std::endl;
    std::cout << "Traditional C++ Execution Time: " << (std::chrono::duration_cast<std::chrono::microseconds>(end_traditional_time-start_traditional_time)).count() << "μs" << std::endl;
    delete output_mult;

    auto start_optimized_time = std::chrono::high_resolution_clock::now();
    output_mult = fl_mult_matrix(&matrix_a, &matrix_b, fl_simd_dot_product);
    auto end_optimized_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "Finished Optimized Multiplication" << std::endl;
    std::cout << "Optimized  Execution Time: " << (std::chrono::duration_cast<std::chrono::microseconds>(end_optimized_time-start_optimized_time)).count() << "μs" << std::endl;
    
    delete output_mult;
}