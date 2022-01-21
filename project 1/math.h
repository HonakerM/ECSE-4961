#include "matrix.h"
#include <cstring>


// SIMD Instructions
#include <xmmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>

//define batch length
// 4 for sse
// 8 for avx
#define SIMD_BATCH_SIZE 8

// function used to calculate the multiplication of matrix using SIMD instructions
matrix<float>* fl_simd_mult_matrix(matrix<float>* a, matrix<float>* b);
float fl_simd_dot_product( const float* a, const float* b, uint size);