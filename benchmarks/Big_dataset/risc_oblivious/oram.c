#include <stdio.h>
#include <stdlib.h>
#include "../../../primitives/path_oram/path_oram.h"

static int Z = 4;
static int C = 200;
static int N = (1 << 20);
static int B = 32;
static int NUM_WRITE = 10;
static int NUM_READ = 10;

int main(){

    ORAM oram;
    Init_ORAM(&oram, Z, N, B, C, BITONIC_SORT);

    int* wr_data = (int*)malloc(sizeof(int) * B);
    for (int i = 0; i < B; i++)
        wr_data[i] = i;

    int idx = rand() % N;
    // do num_access write
    Access_ORAM(&oram, WRITE, idx, wr_data);

    // do num_access read
    int* rd_data = (int*)malloc(sizeof(int) * B);
    Access_ORAM(&oram, READ, idx, rd_data);

    Free_ORAM(&oram);
    free(wr_data);
    free(rd_data);

    return 0;
}
