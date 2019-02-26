#include <stdio.h>
#include <stdlib.h>
#include "../include/asm.h"

static int numVertices = 256;
static int degree = 4;
static int numEdges = 256 * 4;


typedef struct {
    int src;
    int dst;
} Edge;

typedef struct {
    float PR;
    float nextPR;
    int numOutEdges;
    int* outEdges;
} Vertex;

typedef struct{
    Vertex* vertices;
    Edge* edges;
} Graph;

void __attribute__((noinline)) PageRank(Graph* graph, int num_iter){
    for(int h = 0; h < num_iter; h++){
        for(int i = 0; i < numVertices; i++){
            Vertex* vertex = &graph->vertices[i];
            vertex->nextPR = 0.15 / numVertices;

            for(int j = 0; j < degree; j++){
                Edge* edge = &graph->edges[vertex->outEdges[j]];
                vertex->nextPR += graph->vertices[edge->src].PR / graph->vertices[edge->src].numOutEdges * 0.85;    // data dependent
            }
        }
        for(int i = 0; i < numVertices; i++){
            Vertex* vertex = &graph->vertices[i];
            vertex->PR = vertex->nextPR;
        }
    }
}

void ComputeOutgoingEdges(Graph* graph){
    for(int i = 0; i < numEdges; i++){
        Edge* edge = &graph->edges[i];
        graph->vertices[edge->src].numOutEdges++;   // data dependent
    }
}

void PrintGraph(Graph* graph){
    printf("=== Vertices ===\n");
    for(int i = 0; i < numVertices; i++){
        printf("vertex %d has PR = %f\n", i, graph->vertices[i].PR);
    }
}

int main(){
    Graph graph;
    graph.vertices = (Vertex*) malloc(sizeof(Vertex) * numVertices);
    graph.edges = (Edge*) malloc(sizeof(Edge) * numEdges);

    for(int i = 0; i < numVertices; i++){
        graph.vertices[i].PR = 0.25;
        graph.vertices[i].nextPR = 0;
        graph.vertices[i].numOutEdges = 0;
        graph.vertices[i].outEdges = (int*) malloc(sizeof(int) * degree);
    }

    int k = 0;
    for(int i = 0; i < numVertices; i++){
        for(int j = 1; j <= degree; j++){
            graph.vertices[i].outEdges[j-1] = k;
            graph.edges[k].src = i;
            graph.edges[k].dst = (i + j) % numVertices;
            k++;
        }
    }

    ComputeOutgoingEdges(&graph);
    int cnt1, cnt2;

    _rdtsc(cnt1);
    PageRank(&graph, 1);
    _rdtsc(cnt2);
    int t1 = cnt2 - cnt1;

    _rdtsc(cnt1);
    PageRank(&graph, 1);
    _rdtsc(cnt2);
    int t2 = cnt2 - cnt1;

    _rdtsc(cnt1);
    PageRank(&graph, 1);
    _rdtsc(cnt2);
    int t3 = cnt2 - cnt1;

    _rdtsc(cnt1);
    PageRank(&graph, 1);
    _rdtsc(cnt2);
    int t4 = cnt2 - cnt1;

    printf("%d %d %d %d\n", t1,t2,t3,t4);


    return 0;
}



