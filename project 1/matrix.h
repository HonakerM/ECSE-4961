/*
 * Matrix Defintions
*/

#ifndef MATRIX_H
#define MATRIX_H

//general libraries
#include <stdlib.h>
#include <assert.h>
#include <iostream>


//local includes to include SIMD_BATCH_SIZE
#include "main.h"

// define matrix organization macros
// this determines weather a matrix is 
// stored by rows or columns. This
// is important in matrix multiplication
// and cache hits. For example in AxB=C
// you would want A to be organized by rows
// and B to be organized by columns
#define ROW_MAJOR 0x01
#define COL_MAJOR 0x02

// this macro defines the epsilon for float
// comparison. 
#define MATRIX_COMPARISON_EPSILON 1000

//template matrix class
template <typename T>
class matrix {
public:
    /*
     * Constructors & Destructors
     */
    matrix(uint width, uint height, bool fill_data);
    ~matrix();
    void free_matrix();

    /*
     * Accessors
     */
    uint get_num_rows() const { return num_rows;}
    uint get_num_columns() const { return num_columns;}
    uint get_ordering() const { return data_order;}

    const T get_cell(uint row, uint column) const;
    const T* get_row(uint row) const;
    const T* get_column(uint column) const;

    /*
     * Modifierts
     */
    void set_cell(T value, uint row_loc, uint column_loc);
    void set_data_ordering(uint order);

    /*
     * Operator
     */
    template<class U> friend std::ostream& operator<<(std::ostream& os, const matrix<U>& matrix);
    template<class U> friend bool operator==(const matrix<U> a, const matrix<U> b);

private:
    uint num_rows;
    uint num_columns;

    // 
    T ** data;

    // default to column major
    uint data_order = COL_MAJOR;

};


/*
 * Implementation
 */
template <typename T> matrix<T>::matrix(uint width, uint height, bool fill_data){
    // generate padding required for a matrix devisible by 8
    // this is required for and sse or avx instructions
    uint width_size_adjust = 0; 
    if(width % 8 != 0) {
        width_size_adjust = 8 - (width % 8);
    }

    uint height_size_adjust = 0; 
    if(height % 8 != 0) {
        height_size_adjust = 8 - (height % 8);
    }


    //generate array to hold all of the columns
    data = new T*[width + width_size_adjust];

    //for each column
    for(uint i = 0; i < width + width_size_adjust; i++){
        //generate array to hold each row for the column
        T* column = new T[height + height_size_adjust];

        //if fill data then add random values
        for(uint j = 0; j < height + height_size_adjust; j++){
            //if still within the bounds of the matrix
            if(fill_data && j < height && i < width){
                
                //column[j] = j;
                column[j] = (T)rand();

            //else pad 0s
            } else {
                column[j] = 0;
            }

        } 

        //assign column array to location
        data[i] = column;
    }


    //update general class values
    num_columns = width;
    num_rows = height;
}

template <typename T>  matrix<T>::~matrix(){
    free_matrix();
}

template <typename T> void matrix<T>::free_matrix() {
    uint size_adjust = 0; 
    if(get_num_columns() % 8 != 0) {
        size_adjust = 8 - (get_num_columns() % 8);
    }

    // free all of the column arrays
    for(uint i = 0; i < get_num_columns()+size_adjust; i++){
       delete[] data[i];
    }

    // clear column array
    delete[] data;
}

/*
 * Accessors
 */

template <typename T> const T matrix<T>::get_cell(uint row, uint column) const {
    // if data order is row major
    if(data_order == ROW_MAJOR){
        return data[row][column];
    } else {
        return data[column][row];
    }
}

template <typename T> const T* matrix<T>::get_row(uint row) const{
    // if data order is row major
    // directly access the array
    if(data_order == ROW_MAJOR){
        return data[row];

    // else you have to generate the array. This
    // should not be needed as the matrix should be
    // correctly oriented. When this is used the data
    // must be freed afterwards.
    } else {
        T* output_array = new T[get_num_columns()];
        for(uint i=0;i<get_num_columns(); i++) {
            output_array[i] = data[i][row];
        }
        return output_array;
    }
}


template <typename T> const T* matrix<T>::get_column(uint column) const{
    // if data order is column major
    // directly access the array
    if(data_order == COL_MAJOR){
        return data[column];

    // else you have to generate the array. This
    // should not be needed as the matrix should be
    // correctly oriented. When this is used the data
    // must be freed afterwards.
    } else {
        T* output_array = new T[get_num_rows()];
        for(uint i=0;i<get_num_rows(); i++) {
            output_array[i] = data[i][column];
        }
        return output_array;
    }}


/* 
 * Modifiers
 */
template <typename T> void matrix<T>::set_cell(T value, uint row_loc, uint column_loc){
    //bounds checking
    if(row_loc > get_num_rows() || column_loc > get_num_columns()){
        return;
    }

    if(data_order == COL_MAJOR){
        data[column_loc][row_loc] = value;
    } else {
        data[row_loc][column_loc] = value;
    }
}
template <typename T> void matrix<T>::set_data_ordering(uint order){
    //bounds checking
    if(order != ROW_MAJOR && order != COL_MAJOR){
        return ;
    }

    //if order is already correct
    if(data_order == order){
        return;
    }

    // generate padding required for a matrix devisible by 4
    // this is required for and sse or avx instructions
    uint width_size_adjust = 0; 
    if(get_num_columns() % 8 != 0) {
        width_size_adjust = 8 - (get_num_columns() % 8);
    }

    uint height_size_adjust = 0; 
    if(get_num_rows() % 8 != 0) {
        height_size_adjust = 8 - (get_num_rows() % 8);
    }



    if(order == ROW_MAJOR){
        //generate array to hold all of the rows
        T** new_data = new T*[get_num_rows()+height_size_adjust];

        //for each column
        for(uint i = 0; i < get_num_rows()+height_size_adjust; i++){
            //generate array to hold each column for the row
            T* row = new T[get_num_columns()+width_size_adjust];

            for(uint j = 0; j < get_num_columns()+width_size_adjust; j++){
                if(i<get_num_rows() && j<get_num_columns()){
                    row[j] = get_cell(i,j);
                } else{
                    row[j] = 0;
                }
            } 

            //assign row array to location
            new_data[i] = row;
        }

        //free previous data
        free_matrix();
        data = new_data;

    } else {
        //generate array to hold all of the columns
        T** new_data = new T*[get_num_columns()+width_size_adjust];

        //for each column
        for(uint i = 0; i < get_num_columns()+width_size_adjust; i++){
            //generate array to hold each row for the column
            T* column = new T[get_num_rows()+height_size_adjust];

            for(uint j = 0; j < get_num_rows()+height_size_adjust; j++){
                if(i<get_num_columns() && j<get_num_rows()){
                    column[j] = get_cell(i,j);
                } else{
                    column[j] = 0;
                }
            } 

            //assign column array to location
            new_data[i] = column;
        }

        //free previous data
        free_matrix();
        data = new_data;
    }

    data_order = order;
}

template <typename T> std::ostream& operator<<(std::ostream& os, const matrix<T>& matrix) {
    for(uint i = 0; i< matrix.get_num_rows(); i++){

        for(uint j =0; j< matrix.get_num_columns(); j++){
            if(matrix.data_order == COL_MAJOR){
                os << (T)matrix.data[j][i] << " ";
            } else {
                os << (T)matrix.data[i][j] << " ";
            }
        }

        os << "\n";
    }
    return os;
}


template <typename T> bool operator==(const matrix<T> a, const matrix<T> b) {
    //printf("%d %d, %d %d\n",a.get_num_rows(), b.get_num_rows(), a.get_num_columns(), b.get_num_columns());
    if(a.get_num_rows() != b.get_num_rows()){
        return false;
    }
    if(a.get_num_columns() != b.get_num_columns()){
        return false;
    }
    for(uint i = 0; i< a.get_num_rows(); i++){

        for(uint j =0; j< a.get_num_columns(); j++){
            //printf("%f %f ", a.get_cell(i,j), b.get_cell(i, j));
            //printf("%f\n", abs(a.get_cell(i,j) - b.get_cell(i, j)));
            if(abs(a.get_cell(i,j) - b.get_cell(i, j)) > MATRIX_COMPARISON_EPSILON){
                std::cout<<"FAILED MATCH " <<a.get_cell(i,j)  << " " << b.get_cell(i, j) << " "<< abs(a.get_cell(i,j) - b.get_cell(i, j)) <<std::endl;
                return false;
            } 
        }
    }
    return true;
}

#endif



