#include "math.h"

matrix<float>* fl_simd_mult_matrix(matrix<float>* a, matrix<float>* b) {
    assert(a->get_num_columns() == b->get_num_rows());
    uint size = a->get_num_columns();

    a->set_data_ordering(ROW_MAJOR);
    b->set_data_ordering(COL_MAJOR);



    matrix<float>* output = new matrix<float>(a->get_num_rows(), b->get_num_columns(), false); 


    for(uint i = 0; i < size; i ++ ){
        for(uint j = 0; j < size; j ++ ) {
            const float* row = a->get_row(i);
        
            const float* col = b->get_column(j);

            float value = fl_dot_product(col, row, size);

            output->set_cell(value, i, j);
        }
    }
    

    return output;
}

float fl_dot_product( const float* a, const float* b, uint size) {
    // define output
    __m128 sum = _mm_setzero_ps();

    for(uint i=0; i < size; i += 8){
        //load values into destination vector
        __m256 a_reg = _mm256_loadu_ps( &a[i] );
        __m256 b_reg = _mm256_loadu_ps( &b[i] );
         
        //compute the dotproduct and add it to the accumulator
        const __m128 dp = simd_dp(a_reg, b_reg);

		sum = _mm_add_ps( sum , dp );
    }

    //return the lower 32 bytes of the sum
    return _mm_cvtss_f32( sum );
}


const __m128 simd_dp(__m256 a, __m256 b){

    return  _mm256_castps256_ps128(_mm256_dp_ps( a, b, 0xFF ));
    /*
    __m256 xy = _mm256_mul_ps(a, b);

    __m128 xylow  = _mm256_castps256_ps128(xy);   // (__m128d)cast isn't portable
    __m128 xyhigh = _mm256_extractf128_ps(xy, 1);
    __m128 sum1 =   _mm_add_ps(xylow, xyhigh);
    __m128 swapped = _mm_shuffle_ps(sum1, sum1, 0b01);   // or unpackhi
    __m128 dotproduct = _mm_add_ps(sum1, swapped);
    return dotproduct;
   

        
    __m256 xy = _mm256_mul_ps( a, b );
    __m256 temp = _mm256_hadd_ps( xy, xy );
    __m128 hi128 = _mm256_extractf128_ps( temp, 1 );
    return _mm_add_ps( _mm256_castps256_ps128(temp), hi128 );


    // while the simpilest method would be to use 
    // _mm256_dp_ps ; however, it is not the fastst
    // because add/mul/move are so optimized it is actually
    // faster to multiply the terms and then horizonatlly
    // add. See this post for refernce
    //https://stackoverflow.com/questions/6996764/fastest-way-to-do-horizontal-sse-vector-sum-or-other-reduction

    // multiply the two arrays
    __m256 r1 = _mm256_mul_ps(a, b);

    //split the 8 float vectors into 2 vectors with 4 floats
    __m128 vlow  = _mm256_castps256_ps128(r1);
    __m128 vhigh = _mm256_extractf128_ps(r1, 1); // high 128

    // use the sse to add the 4 vecotrs
    vlow  = _mm_add_ps(vlow, vhigh);

    //compute the horizontal sum using shuffying and add
    __m128 shuf = _mm_movehdup_ps(vlow);        // broadcast elements 3,1 to 2,0
    __m128 sums = _mm_add_ps(vlow, shuf);
    shuf        = _mm_movehl_ps(shuf, sums); // high half -> low half
    sums        = _mm_add_ss(sums, shuf);

    return  sums;*/
}
