#include "math.h"



float simd_dot_product( const float* a, const float* b, uint size) {
    // define output
    __m128 sum = _mm_setzero_ps();

    for(uint i=0; i < size / SIMD_BATCH_SIZE; i += SIMD_BATCH_SIZE){
        __m256 a_reg;
        __m256 b_reg;

        //load values into registers
        a_reg = _mm256_loadu_ps( &a[i] );
        b_reg = _mm256_loadu_ps( &b[i] );
         
        //compute the dotproduct
        // only return the lower 4 elements
        // as each element is the same value 
        const __m128 dp = _mm256_castps256_ps128(_mm256_dp_ps( a_reg, b_reg, 0xFF ));

		sum = _mm_add_ps( sum , dp );
    }

    //return the lower 32 bytes of the sum
    return _mm_cvtss_f32( sum );
}


float sisd_dot_product( const float* a, const float* b, uint size) {
    float sum = 0;

    for(uint i=0; i<size; i++){
        sum += a[i] * b[i];
    }

    return sum;
}


int simd_dot_product( const int* a, const int* b, uint size){
    // define output
    __m128i sum = _mm_setzero_si128();

    for(uint i=0; i < size / SIMD_BATCH_SIZE; i += SIMD_BATCH_SIZE){
        
        __m128i a_reg;
        __m128i b_reg;

        //load values into registers
        a_reg = _mm_loadu_si128   ( (const __m128i_u*)&a[i] );
        b_reg = _mm_loadu_si128  ( (const __m128i_u*)&b[i] );
         
        //compute the dotproduct
        // only return the lower 4 elements
        // as each element is the same value 
        //const __m128i dp = ;
        __m128i temp_var = _mm_mullo_epi32(a_reg, b_reg);
        temp_var  =_mm_hadd_epi16(temp_var, temp_var);
        temp_var  =_mm_hadd_epi16(temp_var, temp_var);
        temp_var  =_mm_hadd_epi16(temp_var, temp_var);


		sum = _mm_add_epi16 ( sum , temp_var );
    }

    //get the lowest 32 bytes of the sum and then cast to just the lower 16
    return _mm_cvtsi128_si32 ( sum ) & 0x00FF;
}


int sisd_dot_product( const int* a, const int* b, uint size){
    int sum = 0;

    for(uint i=0; i<size; i++){
        sum += a[i] * b[i];
    }

    return sum;
}