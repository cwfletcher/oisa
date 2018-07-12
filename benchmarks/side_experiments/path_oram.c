#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../../primitives/lib/asm.h"
#include "../../primitives/path_oram/path_oram.h"

void __attribute__((noinline))TestPathORAM(ORAM* oram, int N, int B, int* data){
    Access_ORAM(oram, WRITE, 1, data);
}


void Test(int N, int B){
    printf("@@ Test N = %d, B = %d @@\n", N, B);
    ORAM oram;
    Init_ORAM(&oram, 4, N, B, 200, 1);

    int* data = (int*) calloc(B, sizeof(int));
    Write_Blocks(&oram, 1, data);

    TestPathORAM(&oram, N, B, data);
}

int main(int argc, char** argv){
    assert (argc == 3);
    int N = atoi(argv[1]);
    int B = atoi(argv[2]);

    Test(N, B);

    return 0;
}
