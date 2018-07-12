#include <stdio.h>
#include <stdlib.h>
#include "../../../../primitives/cisc_insts/cisc.h"
#include "../../../../primitives/partpls_path_oram/path_oram.h"

static int Z = 4;
static int C = 200;
static int N = (1 << 20);
static int B = 32;

int main(){

    sec_init(2 * L2_WAY_SIZE);

    ORAM oram;
    Init_ORAM(&oram, Z, N, B, C, BITONIC_SORT);

    int* wr_data = (int*)malloc(sizeof(int) * B);
    for(int i = 0; i < B; i++)
        wr_data[i] = i;

    int id = rand() % N;
    // do num_access write
    
    Access_ORAM(&oram, WRITE, id, wr_data);

    // do num_access read
    int* rd_data = (int*)malloc(sizeof(int) * B);
    Access_ORAM(&oram, READ, id, rd_data);

    Free_ORAM(&oram);
    free(wr_data);
    free(rd_data);

    sec_end();

    return 0;
}
