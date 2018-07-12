#include <stdio.h>
#include <stdlib.h>
#include "../../../../primitives/lib/asm.h"
#include "../../../../primitives/cisc_insts/cisc.h"
#include "../../../../primitives/partpls_path_oram/path_oram.h"

static int numVertices = (1 << 16);
static int degree = 3; // log(V)
static int numEdges = (1 << 16) * 3; // numVertices * log(numVertices)

typedef struct {
    int src;
    int dst;
} Edge;

typedef struct {
    float PR;
    float nextPR;
    int numOutEdges;
} Vertex;

typedef struct{
    ORAM vertices;
    Edge* edges;
} Graph;

void __attribute__((noinline)) PageRank(Graph* graph, int num_iter){
    int temp_nextPR;
    Vertex vertex;
    for(int h = 0; h < num_iter; h++){
        for(int i = 0; i < numVertices; i++){
            /*Vertex* vertex = &graph->vertices[i];*/
            /*vertex->nextPR = 0.15 / numVertices;*/
            Access_ORAM(&graph->vertices, READ, i, (int*)&vertex);
            vertex.nextPR = 0.15 / numVertices;
            Access_ORAM(&graph->vertices, WRITE, i, (int*)&vertex);

            for(int j = 0; j < numEdges; j++){
                printf("edge %d\n", j);
                Edge* edge = &graph->edges[j];

                Access_ORAM(&graph->vertices, READ, edge->src, (int*)&vertex);
                temp_nextPR = vertex.nextPR + vertex.PR / vertex.numOutEdges * 0.85;

                int add_nextPR = (edge->dst == i);
                cmov(add_nextPR, (int*)&temp_nextPR, (int*)&vertex.nextPR);

                Access_ORAM(&graph->vertices, WRITE, edge->src, (int*)&vertex);
            }
        }
        for(int i = 0; i < numVertices; i++){
            Access_ORAM(&graph->vertices, READ, i, (int*)&vertex);
            vertex.PR = vertex.nextPR;
            Access_ORAM(&graph->vertices, WRITE, i, (int*)&vertex);
        }
        printf("iter #%d\n", h);
    }
}

void PrintGraph(Graph* graph){
    Vertex vertex;
    printf("=== Vertices ===\n");
    for(int i = 0; i < numVertices; i++){
        Access_ORAM(&graph->vertices, READ, i, (int*)&vertex);
        printf("vertex %d has PR = %f\n", i, vertex.PR);
    }
}

int main(){
    sec_init(L2_WAY_SIZE);
    Graph graph;
    /*graph.vertices = (Vertex*) malloc(sizeof(Vertex) * numVertices);*/
    Init_ORAM(&graph.vertices, 4, numVertices, sizeof(Vertex) / sizeof(int), 200, BITONIC_SORT);
    graph.edges = (Edge*) malloc(sizeof(Edge) * numEdges);

    for(int i = 0; i < numVertices; i++){
        Vertex temp;
        temp.PR = 0.25;
        temp.nextPR = 0;
        temp.numOutEdges = 3;
        Write_Blocks(&graph.vertices, i, (int*)&temp);
    }

    int k = 0;
    for(int i = 0; i < numVertices; i++){
        for(int j = 1; j <= degree; j++){
            graph.edges[k].src = i;
            graph.edges[k].dst = (i + j) % numVertices;
            k++;
        }
    }

    sim_rdtsc();
    PageRank(&graph, 1);
    sim_rdtsc();
    sec_end();

    return 0;
}



