#ifndef __SORT_HEADER__
#define __SORT_HEADER__

#define ASCENDING   1
#define DESCENDING  0

/******************************************************
 *  Non-Oblivious Sorting Algorithm
 *****************************************************/
// Merge sort for integer array
void MergeSort_Int(int arr[], int N, int dir);

// Merge sort for an array of specific data structure.
// Requirements: 1. Size of individual object is block_size * sizeof(int)
//               2. Number of objects to sort is N
//               3. The integer soring key stays at key_idx * sizeof(int) (4bytes) of each block in arr[]
void MergeSort_Block(int arr[], int N, int key_idx, int dir, int block_sz);

// Merge sort for an array of specific data structure.
// Requirements: 1. same as MergeSort_Block
//               2. same as MergeSort_Block
//               3. The primary sorting key stays at key1_idx * sizeof(int) (4bytes) of each block in arr[], 
//                  the secondary sorting key stays at key2_idx * sizeof(int) (4bytes) of each block in arr[]
void MergeSort_General(int arr[], int N, int block_sz, int key1_idx, int dir1, int key2_idx, int dir2);

// Merge sort for sorting two arrays at a time
// Sorting keys are in arr_main[], and arr_aux[] is sorted exactly the same as arr_main[]
// Requirements: 1. size of individual of arr_main[] is block_sz_main * sizeof(int)
//                                        arr_aux[]  is block_sz_aux  * sizeof(int)
//               2. number of objects to sort is N, same in arr_main[] and arr_aux[]
//               3. The integer sorting key stays at key_idx * sizeof(int) (4bytes) of each block in arr_main[]
void MergeSort_TwoArray(int arr_main[], int arr_aux[], int N, int key_idx, int dir, int block_sz_main, int block_sz_aux);

// Merge sort for sorting two arrays at a time
// Sorting keys are in arr_main[], and arr_aux[] is sorted exactly the same as arr_main[]
// Requirements: 1. same as BitonicSort_TwoArray
//               2. same as BitonicSort_TwoArray
//               3. The primary sorting key stays at key1_idx * sizeof(int) (4bytes) of each block in arr_main[],
//                  the secondary sorting key stays at key2_idx * sizeof(int) (4bytes) of each block in arr_main[]
void MergeSort_TwoArray_TwoKey(int arr_main[], int arr_aux[], int N, int key1_idx, int dir1, int key2_idx, int dir2, int block_sz_main, int block_sz_aux);



/******************************************************
 * Oblivious Sorting Algorithm
 *****************************************************/
// Bitonic sort for integer array
void BitonicSort_Int(int arr[], int N, int dir);

// Bitonic sort for an array of specific data structure
// Requirements: 1. Size of individual object is block_sz * sizeof(int)
//               2. Number of objects to sort is N
//               3. The integer soring key stays at key_idx * sizeof(int) (4bytes) of each block in arr[]
void BitonicSort_Block(int arr[], int N, int key_idx, int dir, int block_sz);

// Bitonic sort for an array of specific data structure.
// Requirements: 1. same as BitonicSort_Block
//               2. same as BitonicSort_Block
//               3. The primary sorting key stays at key1_idx * sizeof(int) (4bytes) of each block in arr[], 
//                  the secondary sorting key stays at key2_idx * sizeof(int) (4bytes) of each block in arr[]
void BitonicSort_General(int arr[], int N, int block_sz, int key1_idx, int dir1, int key2_idx, int dir2);

// Bitonic sort for sorting two arrays at a time
// Sorting keys are in arr_main[], and arr_aux[] is sorted exactly the same as arr_main[]
// Requirements: 1. size of individual of arr_main[] is block_sz_main * sizeof(int)
//                                        arr_aux[]  is block_sz_aux  * sizeof(int)
//               2. number of objects to sort is N, same in arr_main[] and arr_aux[]
//               3. The integer sorting key stays at key_idx * sizeof(int) (4bytes) of each block in arr_main[]
void BitonicSort_TwoArray(int arr_main[], int arr_aux[], int N, int key_idx, int dir, int block_sz_main, int block_sz_aux);

// Bitonic sort for sorting two arrays at a time
// Sorting keys are in arr_main[], and arr_aux[] is sorted exactly the same as arr_main[]
// Requirements: 1. same as BitonicSort_TwoArray
//               2. same as BitonicSort_TwoArray
//               3. The primary sorting key stays at key1_idx * sizeof(int) (4bytes) of each block in arr_main[],
//                  the secondary sorting key stays at key2_idx * sizeof(int) (4bytes) of each block in arr_main[]
void BitonicSort_TwoArray_TwoKey(int arr_main[], int arr_aux[], int N, int key1_idx, int dir1, int key2_idx, int dir2, int block_sz_main, int block_sz_aux);


/******************************************************
 * Oblivious Sorting using PLS
 ******************************************************/
// Merge sort for integer array within PLS
void MergeSort_Int_PLS(unsigned arr, int N, int dir);

// Merge sort for an array of specific data structure within PLS.
// Requirements: 1. Size of individual object is block_size * sizeof(int)
//               2. Number of objects to sort is N
//               3. The integer soring key stays at key_idx * sizeof(int) (4bytes) of each block in arr[]
//void MergeSort_Block(int arr[], int N, int key_idx, int dir, int block_sz);


#endif
