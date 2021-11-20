#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "read_sparse_matrix.c"
#include "coo2csc.c"

int NUM_OF_THREADS = 12;
//struct for thread params
typedef struct{
    csc *A;
    int i, j, k;
    int retval;
} params;

//threads worker function
void *func(void *arg){
    params *p = (params *)arg;
    int l = p->k;
    while(1){
        //find common values in columns i,j
        int sum = 0;
        int p1 = p->A->col_ptr[p->j]; 
        int p2 = p->A->col_ptr[p->i];     //pointers used to scan both columns
        while(p->A->col_ptr[p->j + 1] > p1 && p->A->col_ptr[p->i + 1] > p2){
            if (p->A->row[p1] < p->A->row[p2]) { 
                p1++;
            }
            else if(p->A->row[p2] < p->A->row[p1]){
                p2++;
        
            } else {
                sum++;
                p1++;
                p2++;
            }
        }
        p->retval = sum;
        l += NUM_OF_THREADS;
        if(l >= p->A->nnz) break;
        p += NUM_OF_THREADS;
    }
    return NULL;
}

void pthreads_masked_triangle_counting(coo *C, csc *A){
    int j, k;
    pthread_t *threads = (pthread_t *)malloc(NUM_OF_THREADS * sizeof(pthread_t));
    params *p = (params *)malloc(A->nnz * sizeof(params));

    for(j = 0;j < A->n; j++){
        for(k = A->col_ptr[j];k < A->col_ptr[j + 1]; k++){
            p[k].A = A;
            p[k].i = A->row[k];
            p[k].j = j;
            p[k].k = k;
        }
    }
    for(k = 0;k < NUM_OF_THREADS;k++) pthread_create(&threads[k], NULL, func, (void *)&p[k]);
    for(k = 0;k < NUM_OF_THREADS;k++) pthread_join(threads[k], NULL);
    //remove nonzeros and save in coo C
    C->nnz = 0;
    for(k = 0;k < A->nnz; k++){
        if(p[k].retval != 0){
            C->values[C->nnz] = p[k].retval;
            C->row[C->nnz] = p[k].i;
            C->col[C->nnz] = p[k].j;
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
    if(argc == 3 && atoi(argv[2]) != 0) NUM_OF_THREADS = atoi(argv[2]);
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
    coo C;
    C.n = A.n;
    C.row = (int *)malloc(A.nnz * sizeof(int));
    C.col = (int *)malloc(A.nnz * sizeof(int));
    C.values = (int *)malloc(A.nnz * sizeof(int));
    
    //Benchmarking
    struct timespec start, finish;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);
    pthreads_masked_triangle_counting(&C, &A);
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