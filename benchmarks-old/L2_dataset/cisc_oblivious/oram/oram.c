#include <stdio.h>
#include <stdlib.h>
#include "../../../../primitives/cisc_insts/cisc.h"
#include "../../../../primitives/pls_path_oram/path_oram.h"

static int Z = 4;
static int C = 200;
static int N = (1 << 14);
static int B = 8;
static int NUM_WRITE = 10;
static int NUM_READ = 10;

int main(){

    sec_init(2 * L2_WAY_SIZE);

    ORAM oram;
    Init_ORAM(&oram, Z, N, B, C, BITONIC_SORT);

    int* wr_data = (int*)malloc(sizeof(int) * NUM_WRITE * B);
    for (int i = 0; i < NUM_WRITE * B; i++)
        wr_data[i] = i;

    // do num_access write
    for (int i = 0; i < NUM_WRITE; i++){
        Access_ORAM(&oram, WRITE, i, wr_data + i * B);
    }

    // do num_access read
    int* rd_data = (int*)malloc(sizeof(int) * B);
    for (int i = 0; i < NUM_READ; i++){
        Access_ORAM(&oram, READ, i, rd_data);
    }

    Free_ORAM(&oram);
    free(wr_data);
    free(rd_data);

    sec_end();

    return 0;
}
