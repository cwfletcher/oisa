/*
 * This oblivious PageRank implementation follows GraphSC methodology
 */
#include <stdio.h>
#include <stdlib.h>
#include "../../../../primitives/lib/asm.h"
#include "../../../../primitives/sort/sort.h"

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

void Scatter(tuple* G, int b){
    if (b)  // OUT: src - ASCENDING, isVertex: DESCENDING 
        /*BitonicSort_General((int*)G, N, sizeof(tuple) / sizeof(int), 0, 1, 2, 0);*/
        MergeSort_General((int*)G, N, sizeof(tuple) / sizeof(int), 0, 1, 2, 0);
    else    // IN:  dst - ASCENDING, isVertex: DESCENDING
        /*BitonicSort_General((int*)G, N, sizeof(tuple) / sizeof(int), 1, 1, 2, 0);*/
        MergeSort_General((int*)G, N, sizeof(tuple) / sizeof(int), 1, 1, 2, 0);

    tuple tempVertex;
    tempVertex.v_PR = 1;
    tempVertex.v_numOutEdges = 1;
    tempVertex.v_agg = 1;
    for(int i = 0; i < N; i++){
        int isVertex = G[i].isVertex;
        float fs_val = Fs(&tempVertex);
        cmov(isVertex,  (int*)&G[i].v_PR,    (int*)&tempVertex.v_PR);
        cmov(isVertex,  &G[i].v_numOutEdges, &tempVertex.v_numOutEdges);
        cmov(isVertex,  (int*)&G[i].v_agg,   (int*)&tempVertex.v_agg);
        cmov(!isVertex, (int*)&fs_val,       (int*)&G[i].e_weight); 
    }
}

void Gather(tuple* G, int b){
    if (b)  // OUT: src - ASCENDING, isVertex - ASECNEDING
        /*BitonicSort_General((int*)G, N, sizeof(tuple) / sizeof(int), 0, 1, 2, 1);*/
        MergeSort_General((int*)G, N, sizeof(tuple) / sizeof(int), 0, 1, 2, 1);
    else    // IN:  dst - ASECNDING, isVertex - ASECENDING
        /*BitonicSort_General((int*)G, N, sizeof(tuple) / sizeof(int), 1, 1, 2, 1);*/
        MergeSort_General((int*)G, N, sizeof(tuple) / sizeof(int), 1, 1, 2, 1);

    float agg = 0;
    for(int i = 0; i < N; i++){
        int isVertex = G[i].isVertex;
        float fg_val = Fg(agg, &G[i]);
        cmov(isVertex,  (int*)&agg,    (int*)&G[i].v_agg);
        cmov(isVertex,  (int*)&zero,   (int*)&agg);
        cmov(!isVertex, (int*)&fg_val, (int*)&agg);
    }
}

void Apply(tuple* G){
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
    sim_rdtsc();
    PageRank(G, 1);
    sim_rdtsc();
    PageRank(G, 1);
    sim_rdtsc();
    PageRank(G, 1);
    sim_rdtsc();

    return 0;
}
