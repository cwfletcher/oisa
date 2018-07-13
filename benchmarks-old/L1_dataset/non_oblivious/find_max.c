#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int N = 1024;
static int seed = 0;

// Find the index and value of the largest element in array arr
void __attribute__((noinline))FindMax(int arr[], int* max_idx, int* max_val){
    *max_val = -1;
    for (int i = 0; i < N; i++){
        if (arr[i] > *max_val){
            *max_idx = i;
            *max_val = arr[i];
        }
    }
}


int main(){
    srand(seed);
    int* arr = (int*) malloc(sizeof(int) * N);
    for(int i = 0; i < N; i++){
        arr[i] = rand() % (N * 4);
    }

    int max_idx = 0;
    int max_val = 0;
    FindMax(arr, &max_idx, &max_val);
    printf("The largest element is found at idx = %d, value = %d\n", max_idx, max_val);
    free(arr);

    return 0;
}

