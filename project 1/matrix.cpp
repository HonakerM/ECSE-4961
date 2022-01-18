#include "matrix.h"


fl_matrix::fl_matrix(uint width, uint height, bool fill_data){
    //set static seed for performance testing
    srand(0);

    //generate array to hold all of the columns
    data = (float**)calloc(width, sizeof(float*));

    for(uint i = 0; i < width; i++){
        //generate row for each column
        float* column = (float*)calloc(height, sizeof(float));

        //if fill data then add random values
        if(fill_data){
            for(uint i = 0; i < height; i++){
                column[i] = (float)rand();
            }
        }

        //assign row to column
        data[i] = column;
    }

    num_columns = width;
    num_rows = height;
}

fl_matrix::fl_matrix(float** data, uint width, uint height){
    data = data;
    num_columns = width;
    num_rows = height;
}


uint fl_matrix::get_num_rows() const {
    return num_rows;
}
uint fl_matrix::get_num_columns() const{
    return num_columns;
}

std::ostream& operator<<(std::ostream& os, const fl_matrix& matrix) {
    for(uint i = 0; i< matrix.get_num_rows(); i++){

        for(uint j =0; j< matrix.get_num_columns(); j++){
            os << matrix.data[i][j] << " ";
        }

        os << "\n";
    }
    return os;
}




fl_matrix fl_mult_matrix(fl_matrix a, fl_matrix b) {
    assert(a.num_columns == b.num_rows);


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
