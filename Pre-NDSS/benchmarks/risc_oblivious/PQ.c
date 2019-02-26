#include <stdio.h>
#include <stdlib.h>
#include "PQ.h"
#include "../include/asm.h"

static int seed = 12;
static int N = 1023;
static int NUM = 10;

/// Return True if the first parameter is less than the second
int less_than(int* a, int* b){
    return (*a < *b);
}

void __attribute__((noinline)) Run_PQ(PQ* pq){
    for(int i = 0; i < NUM; i++){
        int val = rand() % N;
        Pop_PQ(pq, 1);
        Push_PQ(pq, &val, 1);
    }
}

int main(){

    srand(seed);

    PQ pq;
    Init_PQ_Full(&pq, N, 1);

    int val = rand() % N;

    Pop_PQ(&pq, 1);
    Push_PQ(&pq, &val, 1);
    Pop_PQ(&pq, 1);
    Push_PQ(&pq, &val, 1);

    Pop_PQ(&pq,1 );
    Push_PQ(&pq, &val, 1);
    Pop_PQ(&pq,1 );
    Push_PQ(&pq, &val, 1);
    Pop_PQ(&pq,1 );
    Push_PQ(&pq, &val, 1);

    Delete_PQ(&pq);
    return 0;
}
