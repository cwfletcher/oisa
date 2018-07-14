#include <stdio.h>
#include <stdlib.h>
#include "../include/asm.h"

#define ASCENDING   1
#define DESCENDING  0

static int N = 256;
static int seed = 0;

void checkPowerOfTwo(int val) {
    while (val % 2 == 0)
        val = val >> 1;
    if (val != 1){
        fprintf(stderr, "ERROR: bitonic sort only works for array with length of power of two!\n");
        /*assert(val == 1);*/
    }
}

void CompAndSwap(int arr[], int i, int j, int dir){
    int act_dir = (arr[i] > arr[j]);
    int if_swap = (dir == act_dir);
    _oswap(if_swap, &arr[i], &arr[j]);
}

void BitonicMerge(int arr[], int low, int cnt, int dir){
    if (cnt > 1){
        int k = cnt / 2;
        for (int i=low; i<low + k; i++)
            CompAndSwap(arr, i, i+k, dir);
        BitonicMerge(arr, low, k, dir);
        BitonicMerge(arr, low+k, k, dir);
    }
}

/// Function: Subroutine: sort
void BitonicSubSort(int arr[], int low, int cnt, int dir){
    if (cnt > 1){
        int k = cnt / 2;
        BitonicSubSort(arr, low, k, 1);
        BitonicSubSort(arr, low+k, k, 0);
        BitonicMerge(arr, low, cnt, dir);
    }
}

//  dir = 1 is ascending order
//     = 0 is descending order
void BitonicSort(int arr[], int N, int dir){
    checkPowerOfTwo(N);
    BitonicSubSort(arr, 0, N, dir);
}

int main(){
    srand(seed);
    int* arr = (int*) malloc(sizeof(int) * N);
    for(int i = 0; i < N; i++)
        arr[i] = N-i;

    printf("Before sorting:\n");
    for(int i = 0; i < N; i++)
        printf("%d, ", arr[i]);
    printf("\n");

    int cnt1, cnt2;
    _rdtsc(cnt1);
    BitonicSort(arr, N, 1);
    _rdtsc(cnt2);
    /*BitonicSort_Int(arr, N, 0);*/
    /*sim_rdtsc();*/
    /*BitonicSort_Int(arr, N, 1);*/
    /*sim_rdtsc();*/
    /*BitonicSort_Int(arr, N, 0);*/
    /*sim_rdtsc();*/
    /*BitonicSort_Int(arr, N, 1);*/
    /*sim_rdtsc();*/

    printf("After sorting:\n");
    for(int i = 0; i < N; i++)
        printf("%d, ", arr[i]);
    printf("\n");

    return 0;
}
