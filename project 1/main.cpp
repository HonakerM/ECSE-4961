// General Imports
#include <stdint.h>
#include <stdio.h>
#include <iostream>

//matrix imports
#include "fl_matrix.h"

fl_matrix* fl_test(fl_matrix* a, fl_matrix* b);

int main(int argc, char *argv[])
{   
    const float in_1[4] = {1,2,3, 4};
    const float in_2[4] = {7, 9, 11, 13};

    float output = fl_simd_dot_product((const float *)&in_1, (const float *)&in_2, 4);
    printf("%f\n",output);

    fl_matrix matrix_a = fl_matrix(10,10, true);
    std::cout << "Matrix 1:" << std::endl;
    std::cout << matrix_a;

    std::cout << "Matrix 2:" << std::endl;
    fl_matrix matrix_b = fl_matrix(10, 10, true);
    std::cout << matrix_b;


    std::cout << "Output:" << std::endl;
    fl_matrix* output_mult = fl_simd_mult_matrix(&matrix_a, &matrix_b);
    std::cout << *output_mult;

    delete output_mult;
}


fl_matrix* fl_test(fl_matrix* a, fl_matrix* b){
    fl_matrix* matrix = new fl_matrix(10,10,false);
    return matrix;
}
