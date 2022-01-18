// General Imports
#include <stdint.h>
#include <stdio.h>

//matrix imports
#include "matrix.h"

int main(int argc, char *argv[])
{   
    const float in_1[4] = {1,2,3, 4};
    const float in_2[4] = {7, 9, 11, 13};

    float output = fl_dot_product((const float *)&in_1, (const float *)&in_2, 4);
    printf("%f\n",output);

    fl_matrix matrix = fl_matrix(2,2, true);
    printf("%f %f\n",matrix.data[0][0], matrix.data[1][0]);
    printf("%f %f\n",matrix.data[0][1], matrix.data[1][1]);

}

