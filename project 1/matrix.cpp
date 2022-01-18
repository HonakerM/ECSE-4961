#include "matrix.h"



struct fl_matrix fl_generate_matrix(uint width, uint height){
    //set static seed for performance testing
    srand(0);

    //generate array to hold all of the columns
    float ** data = (float**)calloc(width, sizeof(float*));

    for(uint i = 0; i < width; i++){
        //generate row for each column
        float* column = (float*)calloc(height, sizeof(float));

        for(uint i = 0; i < height; i++){
            column[i] = (float)rand();
        }

        //assign row to column
        data[i] = column;
    }

    struct fl_matrix output_matrix;
    output_matrix.num_columns = width;
    output_matrix.num_rows = height;
    output_matrix.data = data;
    return output_matrix;
}


struct fl_matrix fl_mult_matrix(struct fl_matrix a, struct fl_matrix b) {
    

}

float fl_dot_product( const float* a, const float* b, int size) {
    // define output
    __m128 sum = _mm_setzero_ps();

    for(int i=0; i< size; i += 4){
        //load values into registers
		const __m128 a_reg = _mm_loadu_ps( &a[i] );
		const __m128 b_reg = _mm_loadu_ps( &b[i] );

        //compute the dotproduct and add it to the accumulator
		const __m128 dp = _mm_dp_ps( a_reg, b_reg, 0xFF );
		sum = _mm_add_ps( sum, dp );
    }

    //return the lower 32 bytes of the sum
    return _mm_cvtss_f32( sum );
}
