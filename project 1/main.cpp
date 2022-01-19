// General Imports
#include <stdint.h>
#include <stdio.h>
#include <iostream>

//matrix imports
#include "fl_matrix.h"

int main(int argc, char *argv[])
{   
    const float in_1[4] = {1,2,3, 4};
    const float in_2[4] = {7, 9, 11, 13};

    float output = fl_simd_dot_product((const float *)&in_1, (const float *)&in_2, 4);
    printf("%f\n",output);

    fl_matrix matrix = fl_matrix(10,10, true);
    std::cout << matrix;
}

