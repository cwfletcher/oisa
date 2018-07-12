#include <stdio.h>
#include <stdlib.h>
#include "../../../../primitives/lib/asm.h"
#include "../../../../primitives/cisc_insts/cisc.h"
#include "../../../../primitives/sort/sort.h"

static int N = 1024 * 4;
static int seed = 0;


int main(){
    sec_init(L2_WAY_SIZE);
    srand(seed);
    unsigned arr = 0;
    sec_new(&arr, sizeof(int) * N);

    for(int i = 0; i < N; i++){
        int arr_i = N-i;
        ocstore(arr_i, i, arr);
    }

    /*printf("Before sorting:\n");*/
    /*for(int i = 0; i < N; i++)*/
        /*printf("%d, ", arr[i]);*/
    /*printf("\n");*/

    sim_rdtsc();
    MergeSort_Int_PLS(arr, N, 1);
    for(int i = 0; i < N; i++){
        int temp;
        ocload(temp, i, arr);
        printf("%d ", temp);
    }
    sim_rdtsc();
    MergeSort_Int_PLS(arr, N, 0);
    for(int i = 0; i < N; i++){
        int temp;
        ocload(temp, i, arr);
        printf("%d ", temp);
    }
    sim_rdtsc();

    /*printf("After sorting:\n");*/
    /*for(int i = 0; i < N; i++)*/
        /*printf("%d, ", arr[i]);*/
    /*printf("\n");*/
    sec_free(arr, sizeof(int) * N);

    sec_end();
    return 0;
}
