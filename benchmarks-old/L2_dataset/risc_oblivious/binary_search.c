/*
 *  Binary search SCAN_ORAM version
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../../primitives/lib/asm.h"
#include "../../../primitives/lib/misc.h"
#include "../../../primitives/scan_oram/scan_oram.h"
#include "../../../primitives/path_oram/path_oram.h"

static int N = 1024 * 16;
static int seed = 0;
static int zero = 0;

int __attribute__((noinline)) BinarySearch_ScanORAM(int* arr, int l, int r, int x, int num_iter){
    int answer = -1;
    /*for(int i = 0; i < num_iter; i++){*/
        /*int valid_op = (r >= l);*/
        /*int m = l + (r-l) / 2;*/

        /*// if r < l, m=0*/
        /*cmov(!valid_op, &zero, &m);*/

        /*// read for arr[m]*/
        /*ScanORAM_Read(arr, N, 1, &arr_m, m);*/

        /*// if arr[m] == x, answer = m*/
        /*int match = (arr_m == x);*/
        /*cmov(match, &m, &answer);*/

        /*// if arr[m] < x, l = m+1*/
        /*int ignore_left = (arr_m < x);*/
        /*int m_plus1 = m + 1;*/
        /*cmov(ignore_left, &m_plus1, &l);*/

        /*// if arr[m] > x, r = m-1*/
        /*int ignore_right = (arr_m > x);*/
        /*int m_minus1 = m - 1;*/
        /*cmov(ignore_right, &m_minus1, &r);*/
    /*}*/
    for(int i = 0; i < N; i++){
        int match = (arr[i] == x);
        cmov(match, &i, &answer);
    }
    return answer;

    return answer;
}
int __attribute__((noinline)) BinarySearch_PathORAM(ORAM* oram, int l, int r, int x, int num_iter){
    int answer = -1;
    int arr_m = -1;
    for(int i = 0; i < num_iter; i++){
        int valid_op = (r >= l);
        int m = l + (r-l) / 2;

        // if r < l, m=0
        cmov(!valid_op, &zero, &m);

        // read for arr[m]
        Access_ORAM(oram, READ, m, &arr_m);

        // if arr[m] == x, answer = m
        int match = (arr_m == x);
        cmov(match, &m, &answer);

        // if arr[m] < x, l = m+1
        int ignore_left = (arr_m < x);
        int m_plus1 = m + 1;
        cmov(ignore_left, &m_plus1, &l);

        // if arr[m] > x, r = m-1
        int ignore_right = (arr_m > x);
        int m_minus1 = m - 1;
        cmov(ignore_right, &m_minus1, &r);
    }

    return answer;
}

void ScanORAM_test(){
    int B = 1;
    int* arr = (int*) malloc(sizeof(int) * N * B);
    for(int i = 0; i < N*B; i++)
        arr[i] = i;

    srand(seed);
    int val = rand() % (2*N);
    int num_iter = log_2_tight(N) + 1;
    int result = BinarySearch_ScanORAM(arr, 0, N-1, val, num_iter);

    (result == -1)? printf("Value %d is not in the array [0:%d].\n", val, N-1) :
                    printf("Value %d is at index %d in the array [0:%d].\n", val, result, N-1);
    free(arr);
}

void PathORAM_test(){
    int Z = 4;
    int B = 1;
    int C = 200;
    ORAM oram;
    Init_ORAM(&oram, Z, N, B, C, BITONIC_SORT);
    for (int i = 0; i < N; i++)
        Access_ORAM(&oram, WRITE, i, &i);

    srand(seed);
    int val = rand() % (2*N);
    int num_iter = log_2_tight(N) + 1;
    int result = BinarySearch_PathORAM(&oram, 0, N-1, val, num_iter);

    (result == -1)? printf("Value %d is not in the array [0:%d].\n", val, N-1) :
                    printf("Value %d is at index %d in the array [0:%d].\n", val, result, N-1);
}

int main(){

    ScanORAM_test();
    return 0;
}
