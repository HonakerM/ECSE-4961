// General Imports
#include <stdint.h>
#include <stdio.h>

// SIMD Instructions
#include <xmmintrin.h>
#include <smmintrin.h>


float float_dot_product( const float* a, const float* b, int size){
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



int main(int argc, char *argv[])
{   
    const float in_1[4] = {1,2,3, 4};
    const float in_2[4] = {7, 9, 11, 13};

    float output = float_dot_product(&in_1, in_2, 4);
    printf("%f",output);

}

