/*
 * Matrix Defintions
*/

//general libraries
#include <stdlib.h>
#include <assert.h>
#include <iostream>

// SIMD Instructions
#include <xmmintrin.h>
#include <smmintrin.h>


class fl_matrix {
public:
    fl_matrix(float** data, uint width, uint height);
    fl_matrix(uint width, uint height, bool fill_data);

    uint get_num_rows() const;
    uint get_num_columns() const;

    const float* get_row(uint row) const;
    const float* get_column(uint column) const;

    void set_row(float* row, uint row_loc);
    void set_column(float* column, uint column_loc);

    friend std::ostream& operator<<(std::ostream& os, const fl_matrix& matrix);

    uint num_rows;
    uint num_columns;
    float ** data;

};




fl_matrix fl_mult_matrix(fl_matrix a, fl_matrix b);
float* get_row(fl_matrix matrix, uint row);
float* get_column(fl_matrix matrix, uint column);
float fl_dot_product( const float* a, const float* b, int size);
