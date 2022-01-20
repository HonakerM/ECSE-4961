// General Imports
#include <stdint.h>
#include <stdio.h>
#include <iostream>

//matrix imports
#include "matrix.h"
#include "math.h"


int main(int argc, char *argv[])
{   
    /*
    const float in_1[4] = {1,2,3, 4};
    const float in_2[4] = {7, 9, 11, 13};

    float output = fl_simd_dot_product((const float *)&in_1, (const float *)&in_2, 4);
    printf("%f\n",output);
    
    */

    matrix<float> matrix_a = matrix<float>(1000,1000, true);

    matrix<float> matrix_b = matrix<float>(1000, 1000, true);

    std::cout << "Finished Geneation" << std::endl;
    matrix<float>* output_mult = fl_simd_mult_matrix(&matrix_a, &matrix_b);

    std::cout << "Finished Multiplication" << std::endl;
    delete output_mult;
}