#include "matrix.h"
#include <cstring>

// function used to calculate the multiplication of matrix using SIMD instructions
matrix<float>* fl_simd_mult_matrix(matrix<float>* a, matrix<float>* b);
float fl_simd_dot_product( const float* a, const float* b, uint size);