#ifndef __PQ_HEADER__
#define __PQ_HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../../../../primitives/lib/asm.h"
#include "../../../../primitives/lib/misc.h"
#include "../../../../primitives/cisc_insts/cisc.h"

static int one = 1;

/// Return True if the first parameter is less than the second
//  Use the real less than logic if you want the root to be the largest node
//  Use the greater/greater_or_equal logic if you want the root to be the smallest node
int less_than(int* a, int* b);

/// Remember that heap[0] is a dummy node
typedef struct {
    int size;       // == number of element in pq
    int capacity;
    int block_size; // == number of words per element
    int max_iter;
    unsigned heap;
} PQ;

void Init_PQ(PQ* pq, int capacity, int block_size){
    pq->size = 0;
    pq->capacity = capacity;
    pq->block_size = block_size;
    pq->max_iter = log_2_tight(capacity);

    sec_new(&pq->heap, sizeof(int) * (capacity+1) * block_size);
}
void Init_PQ_Full(PQ* pq, int capacity, int block_size){
    pq->size = capacity;
    pq->capacity = capacity;
    pq->block_size = block_size;
    pq->max_iter = log_2_tight(capacity);

    sec_new(&pq->heap, sizeof(int) * (capacity+1) * block_size);
    for(int i = 1; i <= capacity; i++){
        int val = capacity - i;
        ocstore(val, i, pq->heap);
    }
}

/// Delete root
void Pop_PQ(PQ* pq, int real){

    unsigned paddr, paddr2;
    // if(real), pq->heap[pq->size] = pq->heap[root]
    // else,     pq->heap[pq->size] = pq->heap[pq->size]
    int root_idx = pq->size;
    cmov(real, &one, &root_idx);

    paddr = pq->heap + pq->block_size * pq->size * sizeof(int);
    paddr2 = pq->heap + pq->block_size * root_idx * sizeof(int);
    memcpy_int_inside_pls(paddr, paddr2, pq->block_size);

    // if(real) pq->size--
    int pqsize_m1 = pq->size - 1;
    cmov(real, &pqsize_m1, &pq->size);

    int k = 1;
    int done = 0;
    cmov(!real, &one, &done);   // if (!real) done = True

    int* item_k     = (int*) malloc(sizeof(int) * pq->block_size);
    int* item_left  = (int*) malloc(sizeof(int) * pq->block_size);
    int* item_right = (int*) malloc(sizeof(int) * pq->block_size);
    memcpy_int_out_pls(pq->heap + k * pq->block_size * sizeof(int), item_k, pq->block_size);

    // Fix down
    for(int i = 0; i < pq->max_iter; i++){
        done = done || !(k*2 < pq->size);

        int left_idx  = k * 2;
        int right_idx = k * 2 + 1;

        memcpy_int_out_pls(pq->heap + left_idx  * pq->block_size * sizeof(int), item_left,  pq->block_size);
        memcpy_int_out_pls(pq->heap + right_idx * pq->block_size * sizeof(int), item_right, pq->block_size);

        int l_lt_r =  less_than(item_left, item_right);
        int k_ge_r = !less_than(item_k, item_right);
        int k_ge_l = !less_than(item_k, item_left);

        done  = done || (l_lt_r && k_ge_r) || (!l_lt_r && k_ge_l);
        int if_swap_k_right = !done &&  l_lt_r && !k_ge_r;
        int if_swap_k_left  = !done && !l_lt_r && !k_ge_l;

        oswap(if_swap_k_left,  &k, &left_idx);
        oswap(if_swap_k_right, &k, &right_idx);

        if (if_swap_k_left)
            memcpy_int_in_pls(item_left,  pq->heap + left_idx  * pq->block_size * sizeof(int), pq->block_size);
        else if (if_swap_k_right)
            memcpy_int_in_pls(item_right, pq->heap + right_idx * pq->block_size * sizeof(int), pq->block_size);
        else
            memcpy_int_in_pls(item_k,     pq->heap + k         * pq->block_size * sizeof(int), pq->block_size);
    }
    memcpy_int_in_pls(item_k,     pq->heap + k * pq->block_size * sizeof(int), pq->block_size);
}

/// Insert an element
void Push_PQ(PQ* pq, int* new_item, int real){
    // if (real) pq->size++
    int pqsize_p1 = pq->size + 1;
    cmov(real, &pqsize_p1, &pq->size);

    assert (pq->size <= pq->capacity);

    unsigned paddr = pq->heap + pq->block_size * pqsize_p1 * sizeof(int);
    memcpy_int_in_pls(new_item, paddr, pq->block_size);

    int k = pq->size;
    int done = 0;
    cmov(!real, &one, &done); // if(!real) done = 1;

    int* item_k      = (int*) malloc(sizeof(int) * pq->block_size);
    int* item_parent = (int*) malloc(sizeof(int) * pq->block_size);
    memcpy_int_out_pls(pq->heap + k * pq->block_size * sizeof(int), item_k, pq->block_size);

    // Fix up
    for(int i = 0; i < pq->max_iter; i++){
        int parent_idx = k / 2;
        done = done || !(k > 1);

        memcpy_int_out_pls(pq->heap + parent_idx * pq->block_size * sizeof(int), item_parent, pq->block_size);

        int parent_lt_k = less_than(item_parent, item_k);
        done = done || !parent_lt_k;
        int if_swap_k_parent = !done && parent_lt_k;

        oswap(if_swap_k_parent, &k, &parent_idx);

        if (if_swap_k_parent)
            memcpy_int_in_pls(item_parent, pq->heap + parent_idx * pq->block_size * sizeof(int), pq->block_size);
        else
            memcpy_int_in_pls(item_k,      pq->heap + k          * pq->block_size * sizeof(int), pq->block_size);
    }
    memcpy_int_in_pls(item_k, pq->heap + k * pq->block_size * sizeof(int), pq->block_size);
}

/// Extract the largest value
void Top_PQ(PQ* pq, int* largest_item){
    unsigned paddr = pq->heap + pq->block_size * sizeof(int);
    memcpy_int_out_pls(paddr, largest_item, pq->block_size);
}

void Delete_PQ(PQ* pq){
    sec_free(pq->heap, sizeof(int) * (pq->capacity+1) * pq->block_size);
}

void Print_PQ(PQ* pq){
    int idx;
    int temp;
    printf("\n=== Print PQ with capacity = %d, size = %d ===\n", pq->capacity, pq->size);
    for(int i = 0; i < pq->size; i++){
        printf("heap[%d]: ", i+1);
        for(int j = 0; j < pq->block_size; j++){
            idx = pq->block_size + i * pq->block_size + j;
            ocload(temp, idx, pq->heap);
            printf("%d ", temp);
        }
        printf("\n");
    }
    printf("=== Print PQ end === \n\n");
}

#endif
