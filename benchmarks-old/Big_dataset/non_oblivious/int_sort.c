#include <stdio.h>
#include <stdlib.h>
#include "../../../primitives/lib/asm.h"
#include "../../../primitives/sort/sort.h"

static int N = (1 << 18);
static int seed = 0;

int main(){
    srand(seed);
    int* arr = (int*) malloc(sizeof(int) * N);
    for(int i = 0; i < N; i++)
        arr[i] = rand() % (2*N);
    
    /*printf("Before sorting:\n");*/
    /*for(int i = 0; i < N; i++)*/
        /*printf("%d, ", arr[i]);*/
    /*printf("\n");*/

    MergeSort_Int(arr, N, 1);
    sim_rdtsc();
    MergeSort_Int(arr, N, 0);
    sim_rdtsc();

    /*printf("After sorting:\n");*/
    /*for(int i = 0; i < N; i++)*/
        /*printf("%d, ", arr[i]);*/
    /*printf("\n");*/

    return 0;
}
