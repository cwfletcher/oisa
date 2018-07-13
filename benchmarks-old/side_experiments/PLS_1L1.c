#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../../primitives/cisc_insts/cisc.h"

static int NUM = 1000;

void __attribute__((noinline)) TestPLS(unsigned paddr, int B, int* addr){
    for(int i = 0; i < NUM; i++)
        memcpy_int_out_pls(paddr, addr, B);
}


void Test(int N, int B){
    printf("@@ Test N = %d, B = %d @@\n", N, B);
    unsigned paddr = 0;
    sec_new(&paddr, N * B * sizeof(int));
    int* data = (int*) calloc(B, sizeof(int));
    for(int i = 0; i < N; i++)
        memcpy_int_in_pls(data, paddr + i * B, B);

    TestPLS(paddr, B, data);

    sec_free(paddr, N * B * sizeof(int));
}

int main(int argc, char** argv){
    sec_init(L1_WAY_SIZE);
    assert (argc == 3);
    int N = atoi(argv[1]);
    int B = atoi(argv[2]);

    Test(N, B);

    sec_end();
    return 0;
}
