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

//floating poitn matrix class
class fl_matrix {
public:
    /*
     * Constructors & Destructors
     */
    fl_matrix(float** data, uint width, uint height);
    fl_matrix(uint width, uint height, bool fill_data);
    ~fl_matrix();

    /*
     * Accessors
     */
    uint get_num_rows() const;
    uint get_num_columns() const;

    const float* get_row(uint row) const;
    const float* get_column(uint column) const;

    /*
     * Modifierts
     */
    void set_cell(float, uint row_loc, uint column_loc);


    /*
     * Operator
     */
    friend std::ostream& operator<<(std::ostream& os, const fl_matrix& matrix);

private:
    uint num_rows;
    uint num_columns;
    float ** data;

};



// function used to calculate the multiplication of matrix using SIMD instructions
fl_matrix fl_simd_mult_matrix(fl_matrix a, fl_matrix b);
float fl_simd_dot_product( const float* a, const float* b, int size);
