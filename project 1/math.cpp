#include "math.h"


matrix<float>* fl_simd_mult_matrix(matrix<float>* a, matrix<float>* b) {
    assert(a->get_num_columns() == b->get_num_rows());
    uint size = a->get_num_columns();

    matrix<float>* output = new matrix<float>(a->get_num_rows(), b->get_num_columns(), false); 

    for(uint i = 0; i < a->get_num_rows(); i ++ ){
        for(uint j = 0; j < b->get_num_columns(); j ++ ) {
            const float* row = a->get_row(i);
        
            const float* col = b->get_column(j);

            float value = fl_simd_dot_product(col, row, size);

            output->set_cell(value, i, j);
        }
    }
    

    return output;
}

float fl_simd_dot_product( const float* a, const float* b, uint size) {
    // define output
    __m128 sum = _mm_setzero_ps();

    for(uint i=0; i< size / 4; i += 4){
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

