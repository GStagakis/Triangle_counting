#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <time.h>
#include "read_sparse_matrix.c"
#include "coo2csc.c"

void cilk_triangle_counting(coo *C, csc *A){
    //********************************Cilk Parallel Implementation******************************************************
    int *temprow = (int *)malloc(A->nnz * sizeof(int));
    int *tempcol = (int *)malloc(A->nnz * sizeof(int));
    int *tempvalues = (int *)malloc(A->nnz * sizeof(int));
    
    cilk_for(int j = 0;j < A->n; j++){
        cilk_for(int k = A->col_ptr[j];k < A->col_ptr[j + 1]; k++){
            int i = A->row[k];   //j = j;
            int sum = 0;
            //find common values in columns i,j
            int p1 = A->col_ptr[j]; int p2 = A->col_ptr[i];                   //pointers used to scan both columns
            while(A->col_ptr[j + 1] > p1 && A->col_ptr[i + 1] > p2){
                if (A->row[p1] < A->row[p2]) { 
                    p1++;
                }
                else if(A->row[p2] < A->row[p1]){
                    p2++;
            
                } else {
                    sum++;
                    p1++;
                    p2++;
                }
            }
            temprow[k] = i;
            tempcol[k] = j;
            tempvalues[k] = sum;
        }
    }
    //remove nonzeros and save in coo C
    C->nnz = 0; int k;
    for(k = 0;k < A->nnz; k++){
        if(tempvalues[k] !=0){
            C->values[C->nnz] = tempvalues[k];
            C->row[C->nnz] = temprow[k];
            C->col[C->nnz] = tempcol[k];
            C->nnz++;
        }
    }
    //fix C size depending on the new number of non zeros
    C->row = (int *)realloc(C->row, C->nnz * sizeof(int));
    C->col = (int *)realloc(C->col, C->nnz * sizeof(int));
    C->values = (int *)realloc(C->values, C->nnz * sizeof(int));
}


int main(int argc, char *argv[]){

    //Function that reads matrix given through terminal in coo form (.mtx format) and prints it in the terminal
    coo mat = read_matrix(argc, argv);

    
    //****************************************I want to convert the matrix to csc form****************************************
    csc A;
    A.n = mat.n;
    A.nnz = mat.nnz;
    A.row = (int *)malloc(A.nnz * sizeof(int));
    A.col_ptr = (int *)malloc((A.n + 1) * sizeof(int));
    coo2csc(A.row, A.col_ptr, mat.row, mat.col, mat.nnz, mat.n, 0);
    free(mat.row);
    free(mat.col);
    //****************************************Initialize C(CSC) matrix********************************************************
    //****************************************C will have same density or less than A*****************************************
    coo C;
    C.n = A.n;
    C.nnz = A.nnz;
    C.row = (int *)malloc(A.nnz * sizeof(int));
    C.col = (int *)malloc(A.nnz * sizeof(int));
    C.values = (int *)malloc(A.nnz * sizeof(int));

    //Benchmarking
    struct timespec start, finish;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);
    cilk_triangle_counting(&C, &A);
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    
    free(A.row);
    free(A.col_ptr);
    print_coo(C);
    printf("\nELAPSED TIME : %f\n", elapsed);
    free(C.row);
    free(C.col);
    free(C.values);
    return 0;
}