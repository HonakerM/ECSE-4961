#include "math.h"

matrix<float>* fl_simd_mult_matrix(matrix<float>* a, matrix<float>* b) {
    //assert that multiplication can happen
    assert(a->get_num_columns() == b->get_num_rows());

    //get size and size adjust
    uint size = a->get_num_columns();
    uint size_adjust = SIMD_BATCH_SIZE - (size % SIMD_BATCH_SIZE);

    a->set_data_ordering(ROW_MAJOR);
    b->set_data_ordering(COL_MAJOR);

    matrix<float>* output = new matrix<float>(a->get_num_rows(), b->get_num_columns(), false); 


    for(uint i = 0; i < size; i ++ ){
        const float* row = a->get_row(i);

        for(uint j = 0; j < size; j ++ ) {
        
            const float* col = b->get_column(j);

            float value = fl_simd_dot_product(col, row, size+size_adjust);

            output->set_cell(value, i, j);
        }
    }
    

    return output;
}

float fl_simd_dot_product( const float* a, const float* b, uint size) {
    // define output
    __m256 sum = _mm256_setzero_ps();

    for(uint i=0; i < size / SIMD_BATCH_SIZE; i += SIMD_BATCH_SIZE){
        __m256 a_reg;
        __m256 b_reg;

        //load values into registers
        a_reg = _mm256_loadu_ps( &a[i] );
        b_reg = _mm256_loadu_ps( &b[i] );
         
        //compute the dotproduct and add it to the accumulator
		const __m256 dp = _mm256_dp_ps( a_reg, b_reg, 0xFF );
		sum = _mm256_add_ps( sum , dp );
    }

    //return the lower 32 bytes of the sum
    return _mm256_cvtss_f32( sum );
}

