#include "math.h"
#include "main.h"

float simd_dot_product( const float* a, const float* b, uint size) {
    // define output
    __m256 sum = _mm256_setzero_ps();

    for(uint i=0; i < size; i += SIMD_BATCH_SIZE){
        __m256 a_reg;
        __m256 b_reg;

        //load values into registers
        a_reg = _mm256_loadu_ps( &a[i] );
        b_reg = _mm256_loadu_ps( &b[i] );
         
        //compute the dotproduct
        const __m256 dp = _mm256_dp_ps( a_reg, b_reg, 0xF1 );

        //add the dotproduct to the sum accumulator
		sum = _mm256_add_ps( sum , dp );
    }

    //
    float returned_vector[SIMD_BATCH_SIZE];
    _mm256_storeu_ps((float*)&returned_vector, sum);
    float veritcal_sum = returned_vector[0] + returned_vector[(SIMD_BATCH_SIZE/2)];

    //return the vertical sum
    return veritcal_sum;
}


float sisd_dot_product( const float* a, const float* b, uint size) {
    float sum = 0;

    for(uint i=0; i < size; i++){
        sum += a[i] * b[i];
    }

    return sum;
}


short simd_dot_product( const short* a, const short* b, uint size){
    // define output
    __m128i sum = _mm_setzero_si128();

    for(uint i=0; i < size; i += SIMD_BATCH_SIZE){
        
        __m128i a_reg;
        __m128i b_reg;

        //load values into registers
        a_reg = _mm_loadu_si128   ( (const __m128i_u*)&a[i] );
        b_reg = _mm_loadu_si128  ( (const __m128i_u*)&b[i] );
         
        //compute the dotproduct
        // only return the lower 4 elements
        // as each element is the same value 
        //const __m128i dp = ;
        __m128i temp_var = _mm_mullo_epi16(a_reg, b_reg);
		sum = _mm_add_epi16 ( sum , temp_var );
    }

    //get array from the __m128i vector
    short returned_vector[SIMD_BATCH_SIZE];
    _mm_storeu_si128((__m128i_u*)&returned_vector, sum);

    //compute the vertical sum
    short veritcal_sum =0;
    for(uint i =0; i < SIMD_BATCH_SIZE; i++){
        veritcal_sum += returned_vector[i];
    }

    //return the vertical sum
    return veritcal_sum;
}


short sisd_dot_product( const short* a, const short* b, uint size){
    short sum = 0;

    for(uint i=0; i < size; i++){
        sum += a[i] * b[i];
    }

    //mask the lowest 16
    return sum;
}