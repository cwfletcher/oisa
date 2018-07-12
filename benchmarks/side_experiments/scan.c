#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../primitives/lib/asm.h"
#include "../../primitives/scan_oram/scan_oram.h"

void __attribute__((noinline)) TestScanORAM(int* arr, int N, int B, int* data){

    ScanORAM_Read(arr, N, B, data, rand() % N);
}

void Test(int N, int B){
    printf("@@ Test N = %d, B = %d @@\n", N, B);
    int* arr = (int*) malloc(sizeof(int) * N * B);
    int* data = (int*) calloc(B, sizeof(int));
    for(int i = 0; i < N; i++)
        memcpy(&arr[i*B], data, sizeof(int) * B);
    sim_rdtsc();
    TestScanORAM(arr, N, B, data);
    sim_rdtsc();
    free(arr);
}

int main(int argc, char**  argv){
    assert (argc == 3);
    int N = atoi(argv[1]);
    int B = atoi(argv[2]);

    Test(N, B);

    return 0;
}
