/*
 * testfile for all inline assembly primitives
 */

#include <stdio.h>
#include <stdlib.h>
#include "asm.h"

int main(){
    int N = 10;
    int S = 4;
    // cmov test
    printf("---- cmov test -----\n");
    int* a = (int*) malloc(sizeof(int) * N * S);
    int* b = (int*) malloc(sizeof(int) * N * S);
    for(int i = 0; i < N*S; i++){
        a[i] = i;
        b[i] = i + N * S;
    }
    for(int i = 0; i < S; i++){
        _cmovn(i%2==0, a+i*N, b+i*N, N);
    }
    for(int i = 0; i < N*S; i++){
        printf("a[%d] = %d,\tb[%d] = %d\n", i, a[i], i, b[i]);
    }
    printf("\n\n");


    // oswap test
    printf("---- oswap test -----\n");
    int* c = (int*) malloc(sizeof(int) * N * S);
    int* d = (int*) malloc(sizeof(int) * N * S);
    for(int i = 0; i < N*S; i++){
        c[i] = i;
        d[i] = i + N * S;
    }
    for(int i = 0; i < S; i++){
        _oswapn(i%2==0, c+i*N, d+i*N, N);
    }
    for(int i = 0; i < N*S; i++){
        printf("c[%d] = %d,\td[%d] = %d\n", i, c[i], i, d[i]);
    }
    printf("\n\n");
    printf("%d\n", sizeof(int));

    free(a);
    free(b);
    free(c);
    free(d);
}
