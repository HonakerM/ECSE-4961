#include "fl_matrix.h"


fl_matrix::fl_matrix(uint width, uint height, bool fill_data){
    //set static seed for performance testing
    srand(0);

    //generate array to hold all of the columns
    //data = (float**)calloc(width, sizeof(float*));
    data_column_primary = new float*[width];

    //for each column
    for(uint i = 0; i < width; i++){
        //generate array to hold each row for the column
        //float* column = (float*)calloc(height, sizeof(float));
        float* column = new float[height];

        //if fill data then add random values
        for(uint j = 0; j < height; j++){
            if(fill_data){
                column[j] = (float)rand();
            } else {
                column[j] = 0;
            }
        } 

        //assign row array to column
        data_column_primary[i] = column;
    }

    data_row_primary = new float*[height];

    //for each column
    for(uint i = 0; i < height; i++){
        //generate array to hold each row for the column
        //float* column = (float*)calloc(height, sizeof(float));
        float* row = new float[width];

        //if fill data then add random values
        for(uint j = 0; j < width; j++){
            row[j] = data_column_primary[j][i];
        } 

        //assign row array to column
        data_row_primary[i] = row;
    }

    //update class values
    num_columns = width;
    num_rows = height;
}

fl_matrix::~fl_matrix(){
    // clear all row arrays
    for(uint i = 0; i < get_num_columns(); i++){
       delete[] data_column_primary[i];
    }

    for(uint i = 0; i < get_num_rows(); i++){
       delete[] data_row_primary[i];
    }

    // clear column array
    delete[] data_column_primary;
    delete[] data_row_primary;

}

/*
 * Accessors
 */
uint fl_matrix::get_num_rows() const {
    return num_rows;
}
uint fl_matrix::get_num_columns() const{
    return num_columns;
}


const float* fl_matrix::get_row(uint row) const{
    //generate output row
    return data_row_primary[row];
}


const float* fl_matrix::get_column(uint column) const{
    return data_column_primary[column];
}


/* 
 * Modifiers
 */
void fl_matrix::set_cell(float value, uint row_loc, uint column_loc){
    data_column_primary[column_loc][row_loc] = value;
    data_row_primary[row_loc][column_loc] = value;
}

std::ostream& operator<<(std::ostream& os, const fl_matrix& matrix) {
    for(uint i = 0; i< matrix.get_num_rows(); i++){

        for(uint j =0; j< matrix.get_num_columns(); j++){
            os << (float)matrix.data_column_primary[j][i] << " ";
        }

        os << "\n";
    }
    return os;
}




fl_matrix* fl_simd_mult_matrix(fl_matrix* a, fl_matrix* b) {
    assert(a->get_num_columns() == b->get_num_rows());
    uint size = a->get_num_columns();

    fl_matrix* output = new fl_matrix(a->get_num_rows(), b->get_num_columns(), false); 

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
