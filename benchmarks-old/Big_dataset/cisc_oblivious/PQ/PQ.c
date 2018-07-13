#include <stdio.h>
#include <stdlib.h>
#include "PQ.h"
#include "../../../../primitives/lib/asm.h"
#include "../../../../primitives/cisc_insts/cisc.h"

static int seed = 0;
static int N = (1 << 20) - 1;
static int NUM = 1;

/// Return True if the first parameter is less than the second
int less_than(unsigned a, unsigned b){
    int va, vb;
    ocload(va, 0, a);
    ocload(vb, 0, b);
    return (va < vb);
}

void __attribute__((noinline)) Run_PQ(PQ* pq){
    for(int i = 0; i < NUM; i++){
        int val = rand() % N;
        Pop_PQ(pq, 1);
        Push_PQ(pq, &val, 1);
    }
}

int main(){

    sec_init(L2_WAY_SIZE);
    srand(seed);

    PQ pq;
    Init_PQ_Full(&pq, N, 1);
    int val = rand() % N;

    sim_rdtsc();
    Run_PQ(&pq);
    sim_rdtsc();

    Delete_PQ(&pq);
    sec_end();

    return 0;
}
