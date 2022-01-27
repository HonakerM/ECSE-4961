#include "matrix.h"
#include "main.h"
#include <cstring>


// SIMD Instructions
#include <xmmintrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>



// function used to calculate the multiplication of matrix using SIMD instructions
float simd_dot_product( const float* a, const float* b, uint size);
float sisd_dot_product( const float* a, const float* b, uint size);

short simd_dot_product( const short* a, const short* b, uint size);
short sisd_dot_product( const short* a, const short* b, uint size);

template<class MATRIX_TYPE> matrix<MATRIX_TYPE>* mult_matrix(matrix<MATRIX_TYPE>* a, matrix<MATRIX_TYPE>* b, bool simd) {
    //assert that multiplication can happen
    assert(a->get_num_columns() == b->get_num_rows());

    //get size and size adjust
    uint size = a->get_num_columns();
    uint size_adjust = 0;
    if(size % SIMD_BATCH_SIZE != 0){
        size_adjust = SIMD_BATCH_SIZE - (size % SIMD_BATCH_SIZE);
    }

#ifdef CACHE_OPTIMIZATION
    a->set_data_ordering(ROW_MAJOR);
    b->set_data_ordering(COL_MAJOR);
#endif

    matrix<MATRIX_TYPE>* output = new matrix<MATRIX_TYPE>(a->get_num_rows(), b->get_num_columns(), false); 


    for(uint i = 0; i < size; i ++ ){
        const MATRIX_TYPE* row = a->get_row(i);

        for(uint j = 0; j < size; j ++ ) {
        
            const MATRIX_TYPE* col = b->get_column(j);

            MATRIX_TYPE value;
            if(simd){
                value = simd_dot_product(col, row, size+size_adjust);
            } else {
                value = sisd_dot_product(col, row, size+size_adjust);
            }

            output->set_cell(value, i, j);
        }
    }
    

    return output;
}



