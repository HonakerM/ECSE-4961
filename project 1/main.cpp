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

    fl_matrix matrix_a = fl_matrix(100,100, true);

    fl_matrix matrix_b = fl_matrix(100, 100, true);

    fl_matrix* output_mult = fl_simd_mult_matrix(&matrix_a, &matrix_b);

    delete output_mult;
}