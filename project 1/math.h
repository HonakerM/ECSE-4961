#include "matrix.h"
#include <cstring>


// SIMD Instructions
#include <xmmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>


// function used to calculate the multiplication of matrix using SIMD instructions
matrix<float>* fl_simd_mult_matrix(matrix<float>* a, matrix<float>* b);
float fl_dot_product( const float* a, const float* b, uint size);
const __m128 simd_dp(__m256* a, __m256* b);