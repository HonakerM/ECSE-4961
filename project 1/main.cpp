// General Imports
#include <stdint.h>
#include <stdio.h>
#include <iostream>

//matrix imports
#include "matrix.h"
#include "math.h"


int main(int argc, char *argv[])
{   
    //set static seed for performance testing
    srand(0);

  
    const float in_1[3] = {1,2,3};
    const float in_2[3] = {1,2,3};

    float output = fl_simd_dot_product((const float *)&in_1, (const float *)&in_2, 3);
    printf("%f\n",output);
    
 
    matrix<float> matrix_a = matrix<float>(10,10, true);
    matrix<float> matrix_b = matrix<float>(10,10, true);

    std::cout << "Finished Geneation" << std::endl;
    std::cout<< matrix_a <<std::endl;
    std::cout<< matrix_b <<std::endl;
    matrix<float>* output_mult = fl_simd_mult_matrix(&matrix_a, &matrix_b);

    std::cout << "Finished Multiplication" << std::endl;
    std::cout << *output_mult;
    delete output_mult;
}