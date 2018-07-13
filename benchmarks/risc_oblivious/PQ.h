#ifndef __OPQ_HEADER__
#define __OPQ_HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../../primitives/lib/asm.h"
#include "../../../primitives/lib/misc.h"
#include "../../../primitives/scan_oram/scan_oram.h"

static int one = 1;


/// Return True if the first parameter is less than the second
int less_than(int* a, int *b);

/// Remember that heap[0] is a dummy node
typedef struct {
    int size;       // size is private
    int capacity;   // capacity is public
    int block_size; // == number of words per element
    int max_iter;
    int*  scan_oram;
} PQ;

void Init_PQ(PQ* pq, int capacity, int block_size){
    pq->size = 0;
    pq->capacity = capacity;
    pq->block_size = block_size;
    pq->max_iter = log_2_tight(capacity);

    pq->scan_oram = (int*) malloc(sizeof(int) * (capacity+1) * block_size);
}

void Init_PQ_Full(PQ* pq, int capacity, int block_size){
    pq->size = capacity;
    pq->capacity = capacity;
    pq->block_size = block_size;
    pq->max_iter = log_2_tight(capacity);

    pq->scan_oram = (int*) malloc(sizeof(int) * (capacity+1) * block_size);
    for(int i = 1; i <= capacity; i++)
        pq->scan_oram[i] = capacity - i;
}

/// Delete root
void Pop_PQ(PQ* pq, int real){

    int* last_item = (int*) malloc(sizeof(int) * pq->block_size);
    ScanORAM_Read (pq->scan_oram, pq->capacity+1, pq->block_size, last_item, pq->size);
    cmovn(real, last_item, &pq->scan_oram[pq->block_size], pq->block_size);

    // if (real) pq->size--
    int pqsize_m1 = pq->size - 1;
    cmov(real, &pqsize_m1, &pq->size);

    int k = 1;
    int done = 0;
    cmov(!real, &one, &done);   // if (!real) done = True

    int* item_k     = (int*) malloc(sizeof(int) * pq->block_size);
    int* item_left  = (int*) malloc(sizeof(int) * pq->block_size);
    int* item_right = (int*) malloc(sizeof(int) * pq->block_size);

    memcpy(item_k, &pq->scan_oram[pq->block_size], pq->block_size * sizeof(int));

    int if_swap_k_right, if_swap_k_left;
    // Fix down
    for(int i = 0; i < pq->max_iter; i++){
        done = done || !(k*2 < pq->size);

        int left_idx  = k * 2;
        int right_idx = k * 2 + 1;

        for(int j = (1 << (i+1)); j < (1 << (i+2)); j++){
            cmovn( (left_idx == j), &pq->scan_oram[pq->block_size * j], item_left,  pq->block_size);
            cmovn((right_idx == j), &pq->scan_oram[pq->block_size * j], item_right, pq->block_size);
        }

        int l_lt_r =  less_than(item_left, item_right);
        int k_ge_r = !less_than(item_k, item_right);
        int k_ge_l = !less_than(item_k, item_left);

        done  = done || (l_lt_r && k_ge_r) || (!l_lt_r && k_ge_l);
        if_swap_k_right = !done &&  l_lt_r && !k_ge_r;
        if_swap_k_left  = !done && !l_lt_r && !k_ge_l;

        oswap(if_swap_k_left,  &k, &left_idx);
        oswap(if_swap_k_right, &k, &right_idx);

        int* item_to_write = NULL;
        for(int j = (1 << i); j < (1 << (i+1)); j++){
            cmov(j == left_idx,  &item_left,  &item_to_write);
            cmov(j == right_idx, &item_right, &item_to_write);
            cmov(j == k,         &item_k,     &item_to_write);

            cmovn(item_to_write != NULL, item_to_write, &pq->scan_oram[pq->block_size * j], pq->block_size);
        }
    }
    ScanORAM_Write(pq->scan_oram, pq->capacity+1, pq->block_size, item_k, k);
}

/// Insert an element
void Push_PQ(PQ* pq, int* new_item, int real){
    // if(real) pq->size++
    int pqsize_p1 = pq->size + 1;
    cmov(real, &pqsize_p1, &pq->size);

    assert (pq->size <= pq->capacity);

    // We leave this unchanged because if not real, it's writing to (size+1) which has no effect
    //ScanORAM_Write(pq->scan_oram, pq->capacity+1, pq->block_size, new_item, pqsize_p1);

    int k = pq->size;
    int done = 0;
    cmov(!real, &one, &done); // if (!real) done = 1;

    int* item_k      = (int*) malloc(sizeof(int) * pq->block_size);
    int* item_parent = (int*) malloc(sizeof(int) * pq->block_size);

    memcpy(new_item, item_k, pq->block_size * sizeof(int));

    // Fix up
    int if_swap_k_parent = 0;
    for(int i = 0; i < pq->max_iter; i++){
        int parent_idx = k / 2;
        done = done || !(k > 1);

        int parent_in_current_level = 0;
        for(int j = (1 << (pq->max_iter-i-1)); j < (1 << (pq->max_iter-i)); j++){
            cmovn(j == parent_idx, &pq->scan_oram[pq->block_size * j], item_parent, pq->block_size);
            parent_in_current_level = parent_in_current_level || (j == parent_idx);
        }

        int parent_lt_k = less_than(item_parent, item_k);
        done = done || (parent_in_current_level && !parent_lt_k);
        if_swap_k_parent = !done && parent_in_current_level && parent_lt_k;

        oswap(if_swap_k_parent, &k, &parent_idx);

        int* item_to_write = NULL;
        for(int j = (1 << (pq->max_iter-i-1)); j < (1 << (pq->max_iter-i)); j++){
            cmov(j == parent_idx, &item_parent, &item_to_write);
            cmov(j == k,          &item_k,      &item_to_write);

            cmovn(item_to_write != NULL, item_to_write, &pq->scan_oram[pq->block_size * j], pq->block_size);
        }
    }
    cmovn(if_swap_k_parent, item_k, &pq->scan_oram[pq->block_size], pq->block_size);
}

/// Extract the largest value
void Top_PQ(PQ* pq, int* largest_item){
    memcpy(largest_item, &pq->scan_oram[pq->block_size], pq->block_size * sizeof(int));
}

void Delete_PQ(PQ* pq){
    free(pq->scan_oram);
}

void Print_PQ(PQ* pq){
    printf("=== Print PQ with capacity = %d, size = %d ===\n", pq->capacity, pq->size);
    for(int i = 0; i < pq->size; i++){
        printf("heap[%d]: ", i+1);
        int* temp = (int*) malloc(sizeof(int) * pq->block_size);
        ScanORAM_Read(pq->scan_oram, pq->capacity+1, pq->block_size, temp, i+1);
        for(int j = 0; j < pq->block_size; j++)
            printf("%d ", temp[j]);
        printf("\n");
    }
}

#endif
