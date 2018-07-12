#ifndef __PQ_HEADER__
#define __PQ_HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../../../../primitives/lib/asm.h"
#include "../../../../primitives/lib/misc.h"
#include "../../../../primitives/partpls_path_oram/path_oram.h"

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
    ORAM heap;
} PQ;

void Init_PQ(PQ* pq, int capacity, int block_size){
    pq->size = 0;
    pq->capacity = capacity;
    pq->block_size = block_size;
    pq->max_iter = log_2_tight(capacity);

    Init_ORAM(&pq->heap, 4, capacity+1, block_size, 200, BITONIC_SORT);
    int* dummy_zero = (int*) malloc(sizeof(int) * block_size);
    Write_Blocks(&pq->heap, 0, dummy_zero);
}
void Init_PQ_Full(PQ* pq, int capacity, int block_size){
    pq->size = capacity;
    pq->capacity = capacity;
    pq->block_size = block_size;
    //pq->max_iter = log_2_tight(capacity);
    pq->max_iter = 4; //log_2_tight(capacity);

    Init_ORAM(&pq->heap, 4, capacity+1, block_size, 200, BITONIC_SORT);
    for(int i = 0; i <= capacity; i++){
        int val = capacity - i;
        Write_Blocks(&pq->heap, i, &val);
    }
}

/// Delete root
void Pop_PQ(PQ* pq, int real){

    // if(real), pq->heap[pq->size] = pq->heap[root]
    // else,     pq->heap[pq->size] = pq->heap[pq->size]
    int root_idx = pq->size;
    cmov(real, &one, &root_idx);

    int* last_item = (int*)malloc(sizeof(int) * pq->block_size);
    Access_ORAM(&pq->heap, READ, pq->size, last_item);
    Access_ORAM(&pq->heap, WRITE, root_idx, last_item);

    // if(real) pq->size--
    int pqsize_m1 = pq->size - 1;
    cmov(real, &pqsize_m1, &pq->size);

    int k = 1;
    int done = 0;
    cmov(!real, &one, &done);   // if (!real) done = True

    int* item_k     = (int*) malloc(sizeof(int) * pq->block_size);
    int* item_left  = (int*) malloc(sizeof(int) * pq->block_size);
    int* item_right = (int*) malloc(sizeof(int) * pq->block_size);

    Access_ORAM(&pq->heap, READ, k, item_k);

    // Fix down
    for(int i = 0; i < pq->max_iter; i++){
        done = done || !(k*2 < pq->size);

        int left_idx  = k * 2;
        int right_idx = k * 2 + 1;
        cmov(done, &k, &left_idx);
        cmov(done, &k, &right_idx);

        Access_ORAM(&pq->heap, READ, left_idx,  item_left);
        Access_ORAM(&pq->heap, READ, right_idx, item_right);

        int l_lt_r =  less_than(item_left, item_right);
        int k_ge_r = !less_than(item_k, item_right);
        int k_ge_l = !less_than(item_k, item_left);

        done  = done || (l_lt_r && k_ge_r) || (!l_lt_r && k_ge_l);
        int if_swap_k_right = !done &&  l_lt_r && !k_ge_r;
        int if_swap_k_left  = !done && !l_lt_r && !k_ge_l;

        oswap(if_swap_k_left,  &k, &left_idx);
        oswap(if_swap_k_right, &k, &right_idx);

        if (if_swap_k_left)
            Access_ORAM(&pq->heap, WRITE, left_idx,  item_left);
        else if (if_swap_k_right)
            Access_ORAM(&pq->heap, WRITE, right_idx, item_right);
        else
            Access_ORAM(&pq->heap, WRITE, k,         item_k);
    }
    Access_ORAM(&pq->heap, WRITE, k,         item_k);
}

/// Insert an element
void Push_PQ(PQ* pq, int* new_item, int real){
    // if (real) pq->size++
    int pqsize_p1 = pq->size + 1;
    cmov(real, &pqsize_p1, &pq->size);

    assert (pq->size <= pq->capacity);

    Access_ORAM(&pq->heap, WRITE, pqsize_p1, new_item);

    int k = pq->size;
    int done = 0;
    cmov(!real, &one, &done); // if(!real) done = 1;

    int* item_k      = (int*) malloc(sizeof(int) * pq->block_size);
    int* item_parent = (int*) malloc(sizeof(int) * pq->block_size);
    Access_ORAM(&pq->heap, READ, k, item_k);

    // Fix up
    for(int i = 0; i < pq->max_iter; i++){
        int parent_idx = k / 2;
        done = done || !(k > 1);
        cmov(done, &k, &parent_idx);

        Access_ORAM(&pq->heap, READ, parent_idx, item_parent);

        int parent_lt_k = less_than(item_parent, item_k);
        done = done || !parent_lt_k;
        int if_swap_k_parent = !done && parent_lt_k;

        oswap(if_swap_k_parent, &k, &parent_idx);

        if (if_swap_k_parent)
            Access_ORAM(&pq->heap, WRITE, parent_idx, item_parent);
        else
            Access_ORAM(&pq->heap, WRITE, k,          item_k);
    }
    Access_ORAM(&pq->heap, WRITE, k, item_k);
}

/// Extract the largest value
void Top_PQ(PQ* pq, int* largest_item){
    Access_ORAM(&pq->heap, READ, 1, largest_item);
}

void Delete_PQ(PQ* pq){
    Free_ORAM(&pq->heap);
}

void Print_PQ(PQ* pq){
    printf("\n=== Print PQ with capacity = %d, size = %d ===\n", pq->capacity, pq->size);
    for(int i = 0; i < pq->size; i++){
        printf("heap[%d]: ", i+1);
        for(int j = 0; j < pq->block_size; j++){
            int* temp = (int*) malloc(sizeof(int) * pq->block_size);
            Access_ORAM(&pq->heap, READ, i+1, temp);
            for(int j = 0; j < pq->block_size; j++)
                printf("%d ", temp[j]);
        }
        printf("\n");
    }
    printf("=== Print PQ end === \n\n");
}

#endif
