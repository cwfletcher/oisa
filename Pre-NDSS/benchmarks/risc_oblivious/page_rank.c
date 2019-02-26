/*
 * This oblivious PageRank implementation follows GraphSC methodology
 */
#include <stdio.h>
#include <stdlib.h>
#include "../include/asm.h"

static int seed = 0;
static int numVertices = 256;
static int degree = 3; // log(V)
static int numEdges = 256 * 3; //numVertices * log(numVertices);
static int N = 256 + 256 * 3;  //numVertices + numEdges;
static int one = 1;
static float zero = 0.;

enum DIRECTION{
    IN,
    OUT
};

typedef struct {
    int src;
    int dst;
    int isVertex;

    // data for vertex
    float v_PR;
    int   v_numOutEdges;
    float v_agg;

    // data for edge
    float e_weight;
    int id;
} tuple;

void PrintGraph(tuple* G){
    for(int i = 0; i < N; i++){
        printf("G[%d].id = %d\n", i, G[i].id);
        printf("G[%d].src = %d\n", i, G[i].src);
        printf("G[%d].dst = %d\n", i, G[i].dst);
        printf("G[%d].isVertex = %d\n", i, G[i].isVertex);
        printf("G[%d].v_PR = %f\n", i, G[i].v_PR);
        printf("G[%d].v_numOutEdges = %d\n", i, G[i].v_numOutEdges);
        printf("G[%d].v_agg = %f\n", i, G[i].v_agg);
        printf("G[%d].e_weight = %f\n", i, G[i].e_weight);
        printf("\n");
    }
}

static inline float Fs(tuple* u){
    return u->v_PR / u->v_numOutEdges;
}
static inline float Fg(float agg, tuple* e){
    return agg + e->e_weight;
}
static inline float Fa(tuple* u){
    return (0.15 / numVertices) + (0.85 * u->v_agg);
}

/************ Sorting functions begin ****************/
static int _key1_idx;
static int _key2_idx;
static int _block_sz;

void checkPowerOfTwo(int val) {
    while (val % 2 == 0)
        val = val >> 1;
    if (val != 1){
        fprintf(stderr, "ERROR: bitonic sort only works for array with length of power of two!\n");
        /*assert(val == 1);*/
    }
}

void CompAndSwap(int arr[], int i, int j, int dir1, int dir2){
    int dir1_eq  = (arr[i*_block_sz+_key1_idx] == arr[j*_block_sz+_key1_idx]);
    int act_dir1 = (arr[i*_block_sz+_key1_idx] >  arr[j*_block_sz+_key1_idx]);
    int act_dir2 = (arr[i*_block_sz+_key2_idx] >  arr[j*_block_sz+_key2_idx]);
    int if_swap = ((!dir1_eq) && (act_dir1 == dir1)) || ((dir1_eq) && (act_dir2 == dir2));
    for (int k = 0; k < _block_sz; k++)
        _oswap(if_swap, &arr[i*_block_sz]+k, &arr[j*_block_sz]+k);
}

void BitonicMerge(int arr[], int low, int cnt, int dir1, int dir2){
    if (cnt > 1){
        int k = cnt / 2;
        for (int i=low; i<low + k; i++)
            CompAndSwap(arr, i, i+k, dir1, dir2);
        BitonicMerge(arr, low, k, dir1, dir2);
        BitonicMerge(arr, low+k, k, dir1, dir2);
    }
}

void BitonicSubSort(int arr[], int low, int cnt, int dir1, int dir2){
    if (cnt > 1){
        int k = cnt / 2;
        BitonicSubSort(arr, low, k, 1, 1);
        BitonicSubSort(arr, low+k, k, 0, 0);
        BitonicMerge(arr, low, cnt, dir1, dir2);
    }
}
void BitonicSort(int arr[], int N, int block_sz, int key1_idx, int dir1, int key2_idx, int dir2){
    _key1_idx = key1_idx;
    _key2_idx = key2_idx;
    _block_sz = block_sz;
    checkPowerOfTwo(N);
    BitonicSubSort(arr, 0, N, dir1, dir2);
}
/*********** Sorting functions end *************/

void __attribute__((noinline)) Scatter(tuple* G, int b){
    if (b)  // OUT: src - ASCENDING, isVertex: DESCENDING 
        BitonicSort((int*)G, N, sizeof(tuple) / sizeof(int), 0, 1, 2, 0);
    else    // IN:  dst - ASCENDING, isVertex: DESCENDING
        BitonicSort((int*)G, N, sizeof(tuple) / sizeof(int), 1, 1, 2, 0);

    tuple tempVertex;
    tempVertex.v_PR = 1;
    tempVertex.v_numOutEdges = 1;
    tempVertex.v_agg = 1;
    for(int i = 0; i < N; i++){
        int isVertex = G[i].isVertex;
        float fs_val = Fs(&tempVertex);
        _cmov(isVertex,  G[i].v_PR,          &tempVertex.v_PR);
        _cmov(isVertex,  G[i].v_numOutEdges, &tempVertex.v_numOutEdges);
        _cmov(isVertex,  G[i].v_agg,         &tempVertex.v_agg);
        _cmov(!isVertex, fs_val,             &G[i].e_weight); 
    }
}

void __attribute__((noinline)) Gather(tuple* G, int b){
    if (b)  // OUT: src - ASCENDING, isVertex - ASECNEDING
        BitonicSort((int*)G, N, sizeof(tuple) / sizeof(int), 0, 1, 2, 1);
    else    // IN:  dst - ASECNDING, isVertex - ASECENDING
        BitonicSort((int*)G, N, sizeof(tuple) / sizeof(int), 1, 1, 2, 1);

    float agg = 0;
    for(int i = 0; i < N; i++){
        int isVertex = G[i].isVertex;
        float fg_val = Fg(agg, &G[i]);
        _cmov(isVertex,  agg,    &G[i].v_agg);
        _cmov(isVertex,  zero,   &agg);
        _cmov(!isVertex, fg_val, &agg);
    }
}

void __attribute__((noinline)) Apply(tuple* G){
    for(int i = 0; i < N; i++)
        G[i].v_PR = Fa(&G[i]);
}

void __attribute__((noinline)) PageRank(tuple* G, int num_iter){
    for(int i = 0; i < num_iter; i++){
        Scatter(G, OUT);
        Gather(G, IN);
        Apply(G);
    }
}

void ComputeOutgoingEdges(tuple* G){
    for(int i = numVertices; i < N; i++){
        int src = G[i].src;
        G[src].v_numOutEdges++;
    }
}

void PrintPR(tuple* G){
    printf("=== Vertices ===\n");
    for(int i = 0; i < N; i++){
        if(G[i].isVertex)
            printf("vertex %d has PR = %f\n", G[i].id, G[i].v_PR);
    }
}

int main(){
    tuple* G = (tuple*) malloc(sizeof(tuple) * N);
    for(int i = 0; i < numVertices; i++){
        G[i].src = i;
        G[i].dst = i;
        G[i].isVertex = 1;
        G[i].v_PR = 1 / numVertices;
        G[i].id = i;
    }
    int k = numVertices;
    for(int i = 0; i < numVertices; i++){
        for(int j = 1; j <=degree; j++){
            G[k].src = i;
            G[k].dst = (i + j) % numVertices;
            G[k].isVertex = 0;
            G[k].id = k;
            k++;
        }
    }

    ComputeOutgoingEdges(G);
    int cnt1, cnt2;
    _rdtsc(cnt1);
    PageRank(G, 1);
    _rdtsc(cnt2);
    int t1 = cnt2 - cnt1;
    _rdtsc(cnt1);
    PageRank(G, 1);
    _rdtsc(cnt2);
    int t2 = cnt2 - cnt1;
    _rdtsc(cnt1);
    PageRank(G, 1);
    _rdtsc(cnt2);
    int t3 = cnt2 - cnt1;

    printf("%d %d %d\n", t1, t2, t3);
    return 0;
}
