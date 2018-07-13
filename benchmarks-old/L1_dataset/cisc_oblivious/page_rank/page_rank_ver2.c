#include <stdio.h>
#include <stdlib.h>
#include "../../../../primitives/lib/asm.h"
#include "../../../../primitives/cisc_insts/cisc.h"

static int numVertices = 256;
static int degree = 3; 
static int numEdges = 256 * 3;

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
    unsigned vertices;
    Edge* edges;
} Graph;

void __attribute__((noinline)) PageRank(Graph* graph, int num_iter){
    int idx;
    float temp_PR = 0;
    float temp_nextPR = 0;
    float new_nextPR = 0;
    int   temp_numOutEdges = 0;
    for(int h = 0; h < num_iter; h++){
        for(int i = 0; i < numVertices; i++){
            /*Vertex* vertex = &graph->vertices[i];*/
            /*vertex->nextPR = 0.15 / numVertices;*/
            temp_nextPR = 0.15 / numVertices;
            idx = i * 3 + 1;
            ocstore(temp_nextPR, idx, graph->vertices);

            for(int j = 0; j < numEdges; j++){
                Edge* edge = &graph->edges[j];
                idx = edge->src * 3;
                ocload(temp_PR, idx, graph->vertices);
                idx++;
                ocload(temp_nextPR, idx, graph->vertices);
                idx++;
                ocload(temp_numOutEdges, idx, graph->vertices);
                new_nextPR = temp_nextPR + temp_PR / temp_numOutEdges * 0.85;

                int add_nextPR = (edge->dst == i);
                cmov(add_nextPR, (int*)&new_nextPR, (int*)&temp_nextPR);

                idx = edge->src * 3 + 1;
                ocstore(temp_nextPR, idx, graph->vertices);
            }
        }
        for(int i = 0; i < numVertices; i++){
            /*Vertex* vertex = &graph->vertices[i];*/
            /*vertex->PR = vertex->nextPR;*/
            idx = i * 3 + 1;
            ocload(temp_nextPR, idx, graph->vertices);
            ocstore(temp_nextPR, idx, graph->vertices);
        }
    }
}

void ComputeOutgoingEdges(Graph* graph){
    int idx;
    int temp_numOutEdges;
    for(int i = 0; i < numEdges; i++){
        Edge* edge = &graph->edges[i];
        //graph->vertices[edge->src].numOutEdges++;    data dependent
        idx = edge->src * 3 + 2;
        ocload(temp_numOutEdges, idx, graph->vertices);
        temp_numOutEdges++;
        ocstore(temp_numOutEdges, idx, graph->vertices);
    }
}

void PrintGraph(Graph* graph){
    float PR = 0;
    int idx;
    printf("=== Vertices ===\n");
    for(int i = 0; i < numVertices; i++){
        idx = i * 3;
        ocload(PR, idx, graph->vertices);
        printf("vertex %d has PR = %f\n", i, PR);
    }
}

int main(){
    sec_init(L1_WAY_SIZE);
    Graph graph;
    sec_new(&graph.vertices, sizeof(Vertex) * numVertices);
    /*graph.vertices = (Vertex*) malloc(sizeof(Vertex) * numVertices);*/
    graph.edges = (Edge*) malloc(sizeof(Edge) * numEdges);

    int idx = 0;
    for(int i = 0; i < numVertices; i++){
        /*graph.vertices[i].PR = 0.25;*/
        float pr = 0.25;
        idx = i * 3;
        ocstore(pr, idx, graph.vertices);
        /*graph.vertices[i].nextPR = 0;*/
        float nextpr = 0.0;
        idx++;
        ocstore(nextpr, idx, graph.vertices);
        /*graph.vertices[i].numOutEdges = 0;*/
        idx++;
        ocstore(0, idx, graph.vertices);
    }

    int k = 0;
    for(int i = 0; i < numVertices; i++){
        for(int j = 1; j <= degree; j++){
            graph.edges[k].src = i;
            graph.edges[k].dst = (i + j) % numVertices;
            k++;
        }
    }

    ComputeOutgoingEdges(&graph);
    PageRank(&graph, 1);
    sec_end();

    return 0;
}



