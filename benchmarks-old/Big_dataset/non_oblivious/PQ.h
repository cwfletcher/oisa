#ifndef __PQ_HEADER__
#define __PQ_HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/// Return True if the first parameter is less than the second
//  Use the real less than logic if you want the root to be the largest node
//  Use the greater/greater_or_equal logic if you want the root to be the smallest node
int less_than(int* a, int* b);

/// Remember that heap[0] is a dummy node
typedef struct {
    int size;
    int capacity;
    int block_size;
    int* heap;
} PQ;

void swap(int* itema, int* itemb, int block_size){
    int temp;
    for(int i = 0; i < block_size; i++){
        temp = itema[i];
        itema[i] = itemb[i];
        itemb[i] = temp;
    }
}

void Init_PQ(PQ* pq, int capacity, int block_size){
    pq->size = 0;
    //pq->size = 0;
    pq->capacity = capacity;
    pq->block_size = block_size;
    pq->heap = (int*)malloc(sizeof(int) * (capacity+1) * block_size);
}

void Init_PQ_Full(PQ* pq, int capacity, int block_size){
    pq->size = capacity;
    //pq->size = 0;
    pq->capacity = capacity;
    pq->block_size = block_size;
    pq->heap = (int*)malloc(sizeof(int) * (capacity+1) * block_size);
    for(int i = 0; i <= capacity; i++)
       pq->heap[i] = capacity-i;
}

/// Delete root
void Pop_PQ(PQ* pq){
    // Cover the root with the last item
    int* last_item = &pq->heap[pq->size * pq->block_size];
    for(int i = 0; i < pq->block_size; i++)
        pq->heap[pq->block_size + i] = last_item[i];
    pq->size--;

    int k = 1;
    // Fix down
    while (k * 2 < pq->size){
        int j = k * 2;
        int* item_k     = &pq->heap[k * pq->block_size];
        int* item_left  = &pq->heap[j * pq->block_size];
        int* item_right = &pq->heap[(j+1) * pq->block_size];
        if (less_than(item_left, item_right)){
            // check the right child
            if (!less_than(item_k, item_right))
                break;
            // swap with right child
            swap(item_k, item_right, pq->block_size); 
            k = j + 1;
        } else {
            // check the left child
            if (!less_than(item_k, item_left))
                break;
            // swap with left child
            swap(item_k, item_left, pq->block_size);
            k = j;
        }
    }
}

/// Insert an element
void Push_PQ(PQ* pq, int* new_item){
    pq->size++;
    assert (pq->size <= pq->capacity);

    int* last_item = &pq->heap[pq->size * pq->block_size]; 
    for(int i = 0; i < pq->block_size; i++)
        last_item[i] = new_item[i];

    int k = pq->size;
    // fix up
    while (k > 1){
        int* item_k      = &pq->heap[k * pq->block_size];
        int* item_parent = &pq->heap[(k/2) * pq->block_size];
        if (less_than(item_parent, item_k)){
            // pop it up
            swap(item_k, item_parent, pq->block_size);
            k = k / 2;
        } else {
            break;
        }
    }
}

/// Extract the largest value
void Top_PQ(PQ* pq, int* largest_item){
    for(int i = 0; i < pq->block_size; i++)
        largest_item[i] = pq->heap[pq->block_size+i];
}

void Delete_PQ(PQ* pq){
    free(pq->heap);
}

void Print_PQ(PQ* pq){
    printf("=== Print PQ with capacity = %d ===\n", pq->capacity);
    for(int i = 0; i < pq->size; i++){
        printf("heap[%d]: ", i+1);
        for(int j = 0; j < pq->block_size; j++){
            printf("%d ", pq->heap[pq->block_size + i * pq->block_size + j]);
        }
        printf("\n");
    }
}

#endif
