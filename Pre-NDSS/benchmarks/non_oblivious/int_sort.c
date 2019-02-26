#include <stdio.h>
#include <stdlib.h>
#include "../include/asm.h"

#define ASCENDING  1
#define DESCENDING 0

// General configs
static int N = 256;
static int seed = 0;

// Static variables for sort
static int* buf_L;
static int* buf_R;

void Merge_Int(int arr[], int l, int m, int r, int dir){
    int i, j, k;
    int len_L = m - l + 1;
    int len_R = r - m;
    
    for(i = 0; i < len_L; i++)
       buf_L[i] = arr[l + i];
    for(j = 0; j < len_R; j++)
       buf_R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    
    while (i < len_L && j < len_R){
        if ( (buf_L[i] <= buf_R[j] && dir == ASCENDING) || (buf_L[i] >= buf_R[j] && dir == DESCENDING) ) {
            arr[k] = buf_L[i];
            i++;
        } else {
            arr[k] = buf_R[j];
            j++;
        }
        k++;
    } 
    while (i < len_L){
        arr[k] = buf_L[i];
        i++;
        k++;
    }
    while (j < len_R){
        arr[k] = buf_R[j];
        j++;
        k++;
    }
}

void MergeSubSort_Int(int arr[], int l, int r, int dir){
    if (l < r) {
        int m = l + (r-l)/2;

        MergeSubSort_Int(arr, l, m, dir);
        MergeSubSort_Int(arr, m+1, r, dir);

        Merge_Int(arr, l, m, r, dir);
    }
}


// dir = 1 is ascending order
//     = 0 is descending order
void MergeSort_Int(int arr[], int N, int dir){
    buf_L = (int*) malloc(sizeof(int) * N);
    buf_R = (int*) malloc(sizeof(int) * N);
    MergeSubSort_Int(arr, 0, N-1, dir);
    free(buf_L);
    free(buf_R);
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
    MergeSort_Int(arr, N, 1);
    _rdtsc(cnt2);
    int t1 = cnt2 - cnt1;

    _rdtsc(cnt1);
    MergeSort_Int(arr, N, 0);
    _rdtsc(cnt2);
    int t2 = cnt2 - cnt1;

    _rdtsc(cnt1);
    MergeSort_Int(arr, N, 1);
    _rdtsc(cnt2);
    int t3 = cnt2 - cnt1;

    _rdtsc(cnt1);
    MergeSort_Int(arr, N, 0);
    _rdtsc(cnt2);
    int t4 = cnt2 - cnt1;

    _rdtsc(cnt1);
    MergeSort_Int(arr, N, 1);
    _rdtsc(cnt2);
    int t5 = cnt2 - cnt1;

    _rdtsc(cnt1);
    MergeSort_Int(arr, N, 0);
    _rdtsc(cnt2);
    int t6 = cnt2 - cnt1;

    printf("After sorting:\n");
    for(int i = 0; i < N; i++)
        printf("%d, ", arr[i]);
    printf("\n");
    printf("%d, %d, %d, %d, %d, %d\n", t1,t2,t3,t4,t5,t6);

    return 0;
}
