#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../../primitives/lib/asm.h"
#include "../../primitives/cisc_insts/cisc.h"
#include "../../primitives/pls_path_oram/path_oram.h"

void __attribute__((noinline)) TestPathORAM(ORAM* oram, int N, int B, int* data){

    Access_ORAM(oram, READ, 0, data);
    Access_ORAM(oram, READ, 1, data);
    Access_ORAM(oram, READ, 2, data);
    Access_ORAM(oram, READ, 3, data);
}


void Test(int N, int B){
    printf("@@ Test N = %d, B = %d @@\n", N, B);
    ORAM oram;
    Init_ORAM(&oram, 4, N, B, 200, 1);

    int* data = (int*) calloc(B, sizeof(int));
    Write_Blocks(&oram, 0, data);
    Write_Blocks(&oram, 1, data);
    Write_Blocks(&oram, 2, data);
    Write_Blocks(&oram, 3, data);

    TestPathORAM(&oram, N, B, data);
}

int main(int argc, char** argv){
    sec_init(2 * L2_WAY_SIZE);
    assert (argc == 3);
    int N = atoi(argv[1]);
    int B = atoi(argv[2]);

    Test(N, B);

    sec_end();
    return 0;
}
