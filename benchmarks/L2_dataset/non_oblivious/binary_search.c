/************************************************************
 *
 *  Copy-paste from geeksforgeeks.org
 *
 ************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int N = 16 * 1024;
static int seed = 0;

int __attribute__((noinline)) BinarySearch(int arr[], int l, int r, int x) {

    while (l <= r) {
        int m = l + (r-l) / 2;

        // Check if x is present at mid
        if (arr[m] == x)
            return m;

        if (arr[m] < x)
            // If x greater, ignore left half
            l = m + 1;
        else
            // If x is smaller, ignore right half
            r = m - 1;

    }

    // if we reach here, then element was not present
    return -1;
}

int main(){
    int* arr = (int*) malloc(sizeof(int) * N);
    for (int i = 0; i < N; i++)
       arr[i] = i;

    srand(seed);
    int val = rand() % N;

    int result = BinarySearch(arr, 0, N-1, val);

    if (result == -1) 
        printf("Value %d is not in the array.\n", val);
    else
        printf("Value %d is at index %d.\n", val, result);

    return 0;
}
