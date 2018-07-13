/*
 *  Oblivious matrix multiplication is the same as non-oblivious matrix multiplication.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static int A_nrow = (1<<8);
static int A_ncol = (1<<8);
static int B_nrow = (1<<8);
static int B_ncol = (1<<8);

void __attribute__((noinline)) MatrixMult(int* A, int* B, int* C){
    int C_nrow = A_nrow;
    int C_ncol = B_ncol;
    assert (A_ncol == B_nrow);
    for(int i = 0; i < C_nrow; i++){
        for(int j = 0; j < C_ncol; j++){
            for(int k = 0; k < A_ncol; k++){
                C[i*C_ncol + j] += A[i * A_ncol + k] * B[k * B_ncol + j];
            }
        }
    }
}

void InitMatrix(int* mat, int nrow, int ncol){
    for(int i = 0; i < nrow * ncol; i++)
        mat[i] = rand() % 9 + 1;
}

void PrintMatrix(int* mat, int nrow, int ncol){
    for(int i = 0; i < nrow; i++){
       for(int j = 0; j < ncol; j++)
          printf("%d, ", mat[i*ncol+j]);
       printf("\n");
    }
}

int main(){

    int C_nrow = A_nrow;
    int C_ncol = B_ncol;
    assert(A_ncol == B_nrow);

    int* A = (int*) malloc(sizeof(int) * A_nrow * A_ncol);
    InitMatrix(A, A_nrow, A_ncol);

    int* B = (int*) malloc(sizeof(int) * B_nrow * B_ncol);
    InitMatrix(B, B_nrow, B_ncol);

    int* C = (int*) malloc(sizeof(int) * C_nrow * C_ncol);

    MatrixMult(A, B, C);

    free(A);
    free(B);
    free(C);

    return 0;
}
