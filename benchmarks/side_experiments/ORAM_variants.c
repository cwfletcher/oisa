/*
 *  Compare oblivious path oram with naive scan oram
 *  Test on different N, B
 *  Give the user the better option
 */
#include <stdio.h>
#include <stdlib.h>
#include "../../primitives/lib/asm.h"
#include "../../primitives/scan_oram/scan_oram.h"
#include "../../primitives/path_oram/path_oram.h"

int numWrite = 100;
int numRead = 100;

void testPathORAM(ORAM* oram, int N, int B, int num_iter){
    int* data = (int*) calloc(B, sizeof(int));

    for(int i = 0; i < N; i++){
        Access_ORAM(oram, WRITE, i, data);
    }
    
    uint64_t begin = rdtsc();

    for(int h = 0; h < num_iter; h++){
        for(int i = 0; i < numWrite; i++){
            Access_ORAM(oram, WRITE, rand() % N, data);
        }
        for(int i = 0; i < numRead; i++){
            Access_ORAM(oram, READ, rand() % N, data);
        }
    }

    uint64_t duration = rdtsc() - begin;
    printf("path oram duration = %llu\n", duration);
}

void testScanORAM(int* arr, int N, int B, int num_iter){
    int* data = (int*) calloc(B, sizeof(int));

    for(int i = 0; i < N; i++){
        ScanORAM_Write(arr, N, B, data, i);
    }

    uint64_t begin = rdtsc();

    for(int h = 0; h < num_iter; h++){
        for(int i = 0; i < numWrite; i++){
            ScanORAM_Write(arr, N, B, data, rand() % N);
        }
        for(int i = 0; i < numRead; i++){
            ScanORAM_Read(arr, N, B, data, rand() % N);
        }
    }

    uint64_t duration = rdtsc() - begin;
    printf("scan oram duration = %llu\n", duration);
}

void test(int N, int B, int num_iter){
    printf("@@ Test N = %d, B = %d @@\n", N, B);
    ORAM oram;
    Init_ORAM(&oram, 4, N, B, 200, 1);
    int* arr = (int*) malloc(sizeof(int) * N * B);
    testPathORAM(&oram, N, B, num_iter);
    testScanORAM(arr, N, B, num_iter);
    Free_ORAM(&oram);
    free(arr);
}

int main(){
    int N [] = {16, 256, 4096, 65536, 1048576};  // 2^4, 2^8, 2^12, 2^16, 2^20
    int B [] = {1, 8, 64, 512, 4096}; // 4B, 32B, 256B, 4KB, 32KB
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            int n = N[i];
            int b = B[j];
            int num_iter = 100000 / n / b;
            num_iter = (num_iter) ? num_iter : 1;
            test(n, b, num_iter);
        }
    }
}
