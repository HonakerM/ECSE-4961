/*
 * Matrix Defintions
*/

//general libraries
#include <stdlib.h>
#include <assert.h>

// SIMD Instructions
#include <xmmintrin.h>
#include <smmintrin.h>


struct fl_matrix {
    uint num_rows;
    uint num_columns;
    float** data;
};

struct fl_matrix fl_generate_matrix(uint width, uint height);
struct fl_matrix fl_mult_matrix(struct fl_matrix a, struct fl_matrix b);
float fl_dot_product( const float* a, const float* b, int size);
