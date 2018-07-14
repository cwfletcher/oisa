/*
 *  Binary search SCAN_ORAM version
 */ 

#include <stdio.h>
#include <stdlib.h>
#include "../include/asm.h"
#include "../include/misc.h"

static int N = 1024;
static int seed = 0;
static int zero = 0;

int __attribute__((noinline)) BinarySearch(int* arr, int x){
    int answer = -1;
    for(int i = 0; i < N; i++){
        int match = (arr[i] == x);
        _cmov(match, i, &answer);
    }

    return answer;
}


int main(){
    int B = 1;
    int* arr = (int*) malloc(sizeof(int) * N * B);
    for(int i = 0; i < N*B; i++)
        arr[i] = i;

    srand(seed);
    int val = rand() % (2*N);
    int cnt1, cnt2;
    _rdtsc(cnt1);
    int res = BinarySearch(arr, val);
    _rdtsc(cnt2);

    (res == -1)?    printf("Value %d is not in the array [0:%d].\n", val, N-1) :
                    printf("Value %d is at index %d in the array [0:%d].\n", val, res, N-1);
    printf("%d\n", cnt2-cnt1);

    free(arr);

    return 0;
}
