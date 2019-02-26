/*************************************************************
 *
 *  Note: this bitonic sort algorithm only works when
 *  N(number of elements to sort) is a power of 2
 *
 *************************************************************/

#include <assert.h>
#include <stdio.h>
#include "../asm.h"
#include "sort.h"

static int _key_idx;
static int _key1_idx;
static int _key2_idx;

static int _block_sz = -1;
static int _block_sz_main = -1;
static int _block_sz_aux  = -1;

static void checkPowerOfTwo(int val) {
    while (val % 2 == 0)
        val = val >> 1;
    if (val != 1){
        fprintf(stderr, "ERROR: bitonic sort only works for array with length of power of two!\n");
        assert(val == 1);
    }
}

/// Function: Compare two elements. IF the order is opposite to dir, swap two elements.
static void CompAndSwap_Int(int arr[], int i, int j, int dir){
    int act_dir = (arr[i] > arr[j]);
    int if_swap = (dir == act_dir);
    oswap(if_swap, &arr[i], &arr[j]);
}

static void CompAndSwap_Block(int arr[], int i, int j, int dir){
    int act_dir = (arr[i*_block_sz+_key_idx] > arr[j*_block_sz+_key_idx]);
    int if_swap = (dir == act_dir);
    for (int k = 0; k < _block_sz; k++)
        oswap(if_swap, &arr[i*_block_sz]+k, &arr[j*_block_sz]+k);
}
static void CompAndSwap_General(int arr[], int i, int j, int dir1, int dir2){
    int dir1_eq  = (arr[i*_block_sz+_key1_idx] == arr[j*_block_sz+_key1_idx]);
    int act_dir1 = (arr[i*_block_sz+_key1_idx] >  arr[j*_block_sz+_key1_idx]);
    int act_dir2 = (arr[i*_block_sz+_key2_idx] >  arr[j*_block_sz+_key2_idx]);
    int if_swap = ((!dir1_eq) && (act_dir1 == dir1)) || ((dir1_eq) && (act_dir2 == dir2));
    for (int k = 0; k < _block_sz; k++)
        oswap(if_swap, &arr[i*_block_sz]+k, &arr[j*_block_sz]+k);
}
static void CompAndSwap_TwoArray(int arr_main[], int arr_aux[], int i, int j, int dir){
    int act_dir = (arr_main[i*_block_sz_main+_key_idx] > arr_main[j*_block_sz_main+_key_idx]);
    int if_swap = (dir == act_dir);
    for (int k = 0; k < _block_sz_main; k++)
        oswap(if_swap, &arr_main[i*_block_sz_main]+k, &arr_main[j*_block_sz_main]+k);
    for (int k = 0; k < _block_sz_aux; k++)
        oswap(if_swap, &arr_aux[i*_block_sz_aux]+k, &arr_aux[j*_block_sz_aux]+k);
}
static void CompAndSwap_TwoArray_TwoKey(int arr_main[], int arr_aux[], int i, int j, int dir1, int dir2){
    int dir1_eq  = (arr_main[i*_block_sz_main+_key1_idx] == arr_main[j*_block_sz_main+_key1_idx]);
    int act_dir1 = (arr_main[i*_block_sz_main+_key1_idx] >  arr_main[j*_block_sz_main+_key1_idx]);
    int act_dir2 = (arr_main[i*_block_sz_main+_key2_idx] >  arr_main[j*_block_sz_main+_key2_idx]);
    int if_swap = ((!dir1_eq) && (act_dir1 == dir1)) || ((dir1_eq) && (act_dir2 == dir2));
    for (int k = 0; k < _block_sz_main; k++)
        oswap(if_swap, &arr_main[i*_block_sz_main]+k, &arr_main[j*_block_sz_main]+k);
    for (int k = 0; k < _block_sz_aux; k++)
        oswap(if_swap, &arr_aux[i*_block_sz_aux]+k, &arr_aux[j*_block_sz_aux]+k);
}


/// Function: Subroutine: merge
static void BitonicMerge_Int(int arr[], int low, int cnt, int dir){
    // TODO: improve by adding base case cnt = 4/8 and using vector operation
    if (cnt > 1){
        int k = cnt / 2;
        for (int i=low; i<low + k; i++)
            CompAndSwap_Int(arr, i, i+k, dir);
        BitonicMerge_Int(arr, low, k, dir);
        BitonicMerge_Int(arr, low+k, k, dir);
    }
}
static void BitonicMerge_Block(int arr[], int low, int cnt, int dir){
    if (cnt > 1){
        int k = cnt / 2;
        for (int i=low; i<low + k; i++)
            CompAndSwap_Block(arr, i, i+k, dir);
        BitonicMerge_Block(arr, low, k, dir);
        BitonicMerge_Block(arr, low+k, k, dir);
    }
}
static void BitonicMerge_General(int arr[], int low, int cnt, int dir1, int dir2){
    if (cnt > 1){
        int k = cnt / 2;
        for (int i=low; i<low + k; i++)
            CompAndSwap_General(arr, i, i+k, dir1, dir2);
        BitonicMerge_General(arr, low, k, dir1, dir2);
        BitonicMerge_General(arr, low+k, k, dir1, dir2);
    }
}
static void BitonicMerge_TwoArray(int arr_main[], int arr_aux[], int low, int cnt, int dir){
    if (cnt > 1){
        int k = cnt / 2;
        for (int i=low; i<low + k; i++)
            CompAndSwap_TwoArray(arr_main, arr_aux, i, i+k, dir);
        BitonicMerge_TwoArray(arr_main, arr_aux, low, k, dir);
        BitonicMerge_TwoArray(arr_main, arr_aux, low+k, k, dir);
    }
}
static void BitonicMerge_TwoArray_TwoKey(int arr_main[], int arr_aux[], int low, int cnt, int dir1, int dir2){
    if (cnt > 1){
        int k = cnt / 2;
        for (int i=low; i<low + k; i++)
            CompAndSwap_TwoArray_TwoKey(arr_main, arr_aux, i, i+k, dir1, dir2);
        BitonicMerge_TwoArray_TwoKey(arr_main, arr_aux, low, k, dir1, dir2);
        BitonicMerge_TwoArray_TwoKey(arr_main, arr_aux, low+k, k, dir1, dir2);
    }
}


/// Function: Subroutine: sort
static void BitonicSubSort_Int(int arr[], int low, int cnt, int dir){
    if (cnt > 1){
        int k = cnt / 2;
        BitonicSubSort_Int(arr, low, k, 1);
        BitonicSubSort_Int(arr, low+k, k, 0);
        BitonicMerge_Int(arr, low, cnt, dir);
    }
}
static void BitonicSubSort_Block(int arr[], int low, int cnt, int dir){
    if (cnt > 1){
        int k = cnt / 2;
        BitonicSubSort_Block(arr, low, k, 1);
        BitonicSubSort_Block(arr, low+k, k, 0);
        BitonicMerge_Block(arr, low, cnt, dir);
    }
}
static void BitonicSubSort_General(int arr[], int low, int cnt, int dir1, int dir2){
    if (cnt > 1){
        int k = cnt / 2;
        BitonicSubSort_General(arr, low, k, 1, 1);
        BitonicSubSort_General(arr, low+k, k, 0, 0);
        BitonicMerge_General(arr, low, cnt, dir1, dir2);
    }
}
static void BitonicSubSort_TwoArray(int arr_main[], int arr_aux[], int low, int cnt, int dir){
    if (cnt > 1){
        int k = cnt / 2;
        BitonicSubSort_TwoArray(arr_main, arr_aux, low, k, 1);
        BitonicSubSort_TwoArray(arr_main, arr_aux, low+k, k, 0);
        BitonicMerge_TwoArray(arr_main, arr_aux, low, cnt, dir);
    }
}
static void BitonicSubSort_TwoArray_TwoKey(int arr_main[], int arr_aux[], int low, int cnt, int dir1, int dir2){
    if (cnt > 1){
        int k = cnt / 2;
        BitonicSubSort_TwoArray_TwoKey(arr_main, arr_aux, low, k, 1, 1);
        BitonicSubSort_TwoArray_TwoKey(arr_main, arr_aux, low+k, k, 0, 0);
        BitonicMerge_TwoArray_TwoKey(arr_main, arr_aux, low, cnt, dir1, dir2);
    }
}


/// Callable functions. Currently three versions
//  dir = 1 is ascending order
//     = 0 is descending order
void BitonicSort_Int(int arr[], int N, int dir){
    checkPowerOfTwo(N);
    BitonicSubSort_Int(arr, 0, N, dir);
}

void BitonicSort_Block(int arr[], int N, int key_idx, int dir, int block_sz){
    _key_idx = key_idx;
    _block_sz = block_sz;
    checkPowerOfTwo(N);
    BitonicSubSort_Block(arr, 0, N, dir);
}

void BitonicSort_General(int arr[], int N, int block_sz, int key1_idx, int dir1, int key2_idx, int dir2){
    _key1_idx = key1_idx;
    _key2_idx = key2_idx;
    _block_sz = block_sz;
    checkPowerOfTwo(N);
    BitonicSubSort_General(arr, 0, N, dir1, dir2);
}

void BitonicSort_TwoArray(int arr_main[], int arr_aux[], int N, int key_idx, int dir, int block_sz_main, int block_sz_aux){
    _key_idx = key_idx;
    _block_sz_main = block_sz_main;
    _block_sz_aux  = block_sz_aux;
    checkPowerOfTwo(N);
    BitonicSubSort_TwoArray(arr_main, arr_aux, 0, N, dir);
}

void BitonicSort_TwoArray_TwoKey(int arr_main[], int arr_aux[], int N, int key1_idx, int dir1, int key2_idx, int dir2, int block_sz_main, int block_sz_aux){
    _key1_idx = key1_idx;
    _key2_idx = key2_idx;
    _block_sz_main = block_sz_main;
    _block_sz_aux  = block_sz_aux;
    checkPowerOfTwo(N);
    BitonicSubSort_TwoArray_TwoKey(arr_main, arr_aux, 0, N, dir1, dir2);
}
