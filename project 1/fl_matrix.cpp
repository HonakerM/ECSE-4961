#include "fl_matrix.h"


fl_matrix::fl_matrix(uint width, uint height, bool fill_data){
    //set static seed for performance testing
    srand(0);

    //generate array to hold all of the columns
    //data = (float**)calloc(width, sizeof(float*));
    data = new float*[width];
    
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
        data[i] = column;

    }

    //update class values
    num_columns = width;
    num_rows = height;
}

fl_matrix::fl_matrix(float** data, uint width, uint height){
    // set class values
    data = data;
    num_columns = width;
    num_rows = height;
}

fl_matrix::~fl_matrix(){
    // clear all row arrays
    for(uint i = 0; i < get_num_columns(); i++){
       delete[] data[i];
    }

    // clear column array
    delete[] data;
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
    float* output_row = new float[get_num_columns()];

    //get value of row
    for(uint i =0; i < get_num_columns(); i++){
        output_row[i] = data[i][row];
    }

    return (const float*)output_row;
}


const float* fl_matrix::get_column(uint column) const{
    return data[column];
}


/* 
 * Modifiers
 */
void fl_matrix::set_cell(float value, uint row_loc, uint column_loc){
    data[column_loc][row_loc] = value;
}

std::ostream& operator<<(std::ostream& os, const fl_matrix& matrix) {
    for(uint i = 0; i< matrix.get_num_rows(); i++){

        for(uint j =0; j< matrix.get_num_columns(); j++){
            os << (float)matrix.data[j][i] << " ";
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

            //get_row allocates space for new row which must be freed
            delete[] row;
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
