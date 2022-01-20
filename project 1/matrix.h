/*
 * Matrix Defintions
*/

#ifndef MATRIX_H
#define MATRIX_H

//general libraries
#include <stdlib.h>
#include <assert.h>
#include <iostream>

// SIMD Instructions
#include <xmmintrin.h>
#include <smmintrin.h>

//floating poitn matrix class
template <typename T>
class matrix {
public:
    /*
     * Constructors & Destructors
     */
    matrix(uint width, uint height, bool fill_data);
    ~matrix();

    /*
     * Accessors
     */
    uint get_num_rows() const;
    uint get_num_columns() const;

    const T* get_row(uint row) const;
    const T* get_column(uint column) const;

    /*
     * Modifierts
     */
    void set_cell(T value, uint row_loc, uint column_loc);

    /*
     * Operator
     */
    template<class U> friend std::ostream& operator<<(std::ostream& os, const matrix<U>& matrix);

private:
    uint num_rows;
    uint num_columns;

    // see constructor for reasoning
    T ** data_row_primary;
    T ** data_column_primary;

};


/*
 * Implementation
 */
template <typename T> matrix<T>::matrix(uint width, uint height, bool fill_data){
    //generate array to hold all of the columns
    data_column_primary = new T*[width];

    //for each column
    for(uint i = 0; i < width; i++){
        //generate array to hold each row for the column
        T* column = new T[height];

        //if fill data then add random values
        for(uint j = 0; j < height; j++){
            if(fill_data){
                column[j] = j+1;
                //column[j] = (T)rand();
            } else {
                column[j] = 0;
            }
        } 

        //assign column array to location
        data_column_primary[i] = column;
    }


    // duplicate the column table but with rows instead of columns
    // this doubles the memory footprint of the application but 
    // greatly speeds up getting arrays for SIMD multiplication
    data_row_primary = new T*[height];

    //for each row
    for(uint i = 0; i < height; i++){
        //generate array to hold each column for the row
        float* row = new T[width];

        //copy the data from the column matrix
        for(uint j = 0; j < width; j++){
            row[j] = data_column_primary[j][i];
        } 

        //asassign row array to locationsign 
        data_row_primary[i] = row;
    }

    //update general class values
    num_columns = width;
    num_rows = height;
}

template <typename T> matrix<T>::~matrix(){
    // free all of the column arrays
    for(uint i = 0; i < get_num_columns(); i++){
       delete[] data_column_primary[i];
    }

    // free all of the row arrays
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
template <typename T> uint matrix<T>::get_num_rows() const {
    return num_rows;
}
template <typename T> uint matrix<T>::get_num_columns() const{
    return num_columns;
}


template <typename T> const T* matrix<T>::get_row(uint row) const{
    return data_row_primary[row];
}


template <typename T> const T* matrix<T>::get_column(uint column) const{
    return data_column_primary[column];
}


/* 
 * Modifiers
 */
template <typename T> void matrix<T>::set_cell(T value, uint row_loc, uint column_loc){
    data_column_primary[column_loc][row_loc] = value;
    data_row_primary[row_loc][column_loc] = value;
}

template <typename T> std::ostream& operator<<(std::ostream& os, const matrix<T>& matrix) {
    for(uint i = 0; i< matrix.get_num_rows(); i++){

        for(uint j =0; j< matrix.get_num_columns(); j++){
            os << (T)matrix.data_column_primary[j][i] << " ";
        }

        os << "\n";
    }
    return os;
}

#endif



