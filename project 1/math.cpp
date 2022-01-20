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
    // if size is not a multiple of 4 then adjust the multiplication
    // do not adjust the entire array as that will require copying
    // all of its contents when really we only need to copy the last 
    // few bytes
    uint size_adjust = 0;
    float* adjusted_a;
    float* adjusted_b;
    if(size % 4 != 0){
        size_adjust = 4 - size % 4;

        size = size_adjust + size;
    }
    

    // define output
    __m128 sum = _mm_setzero_ps();

    for(uint i=0; i < size / 4; i += 4){
        __m128 a_reg;
        __m128 b_reg;


        //if the size was adjusted and we're on the final dp
        // then create the new padded arrays
        if(size_adjust != 0 && i+4 >= size / 4){
            //generate new padded array
            adjusted_a = new float[4];
            adjusted_b = new float[4];

            //copy old values into new value
            std::memcpy(adjusted_a, &a[i], (4-size_adjust) * sizeof(float));
            std::memcpy(adjusted_b, &b[i], (4-size_adjust) * sizeof(float));

            //initalize padded valeus to 0
            for(uint j=4-size_adjust; j < 4; j++){
                adjusted_a[j] = 0;
                adjusted_b[j] = 0;
            }

            //load values into registers
    		a_reg = _mm_loadu_ps( adjusted_a );
    		b_reg = _mm_loadu_ps( adjusted_b );

        }  else {

            //load values into registers
            a_reg = _mm_loadu_ps( &a[i] );
            b_reg = _mm_loadu_ps( &b[i] );
        } 
         
        //compute the dotproduct and add it to the accumulator
		const __m128 dp = _mm_dp_ps( a_reg, b_reg, 0xFF );
		sum = _mm_add_ps( sum, dp );


        //if we adjusted the size then free the space
        if(size_adjust != 0 && i+4 >= size / 4){
            delete[] adjusted_a;
            delete[] adjusted_b;
        }
    }

    //return the lower 32 bytes of the sum
    return _mm_cvtss_f32( sum );
}

