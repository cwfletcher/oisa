/*
 *  General Dijkstra algorithm
 *  Copy-paste from GeeksforFeeks
 *  Rememeber to test with Sparse graph to compare with oblivious dijkstra
 */

#include <limits.h>
#include "PQ.h"

static int Degree = 4;
static int nVertices = (1 << 7) - 1;
static int nEdges = ((1 << 7) - 1) * 4; // nVertices * Degree

typedef struct {
    int*  nOutDegrees;
    int** adjlist;
    int** weights;
} Graph;

void Init_Graph(Graph* graph){
    graph->nOutDegrees = (int*)  malloc(sizeof(int)  * nVertices);
    graph->adjlist     = (int**) malloc(sizeof(int*) * nVertices);
    graph->weights     = (int**) malloc(sizeof(int*) * nVertices);

    for(int i = 0; i < nVertices; i++){
        graph->nOutDegrees[i] = 0;
        graph->adjlist[i]     = (int*) malloc(sizeof(int) * nVertices);
        graph->weights[i]     = (int*) malloc(sizeof(int) * nVertices);
    }
}

void Print_Graph(Graph* graph){
    for(int i = 0; i < nVertices; i++){
        printf("Vertex %d (degree = %d): \n", i, graph->nOutDegrees[i]);
        for(int j = 0; j < graph->nOutDegrees[i]; j++){
            int v      = graph->adjlist[i][j];
            int weight = graph->weights[i][j];
            printf("\t(%d) adjacent vertex %d, distance = %d\n", j, v, weight);
        }
    }
}

int less_than(int* a, int* b){
    return (a[1] > b[1]);
}

// Non-oblivious Dijkstra's algorithm
void __attribute__((noinline)) Dijkstra(Graph* graph, int src, int* dist){

    for(int i = 0; i < nVertices; i++)
        dist[i] = INT_MAX;
    dist[src] = 0;

    PQ pq;
    Init_PQ(&pq, nVertices, 2);
    int src_node [2] = {src, 0};
    Push_PQ(&pq, src_node);

    while(pq.size > 0){
        // The first vertex extracted from PQ is the minimum distance vertex.
        int top_node [2] = {0, 0}; 
        Top_PQ(&pq, top_node);
        Pop_PQ(&pq);
        int u = top_node[0];

        for(int i = 0; i < graph->nOutDegrees[u]; i++){ // data dependent
            // get vertex index and weight of current adjacent of u
            int v = graph->adjlist[u][i];   // data dependent
            int weight = graph->weights[u][i];  // data dependent

            // If this vertex is closer
            if(dist[v] > dist[u] + weight){ // data dependent
                dist[v] = dist[u] + weight;
                int new_node [2] = {v, dist[v]};
                Push_PQ(&pq, new_node);
            }
        }
    }
}

void AddEdge(Graph* graph, int u, int v, int weight){
    assert (u != v);
    int nout;
    // add an edge from u to v
    nout = graph->nOutDegrees[u];
    graph->adjlist[u][nout] = v;
    graph->weights[u][nout] = weight;
    graph->nOutDegrees[u]++;

    // add an edge from v to u
    nout = graph->nOutDegrees[v];
    graph->adjlist[v][nout] = u;
    graph->weights[v][nout] = weight;
    graph->nOutDegrees[v]++;
}

int main(){
    Graph graph;
    // create the graph
    Init_Graph(&graph);

    // makie graph
    for(int i = 0; i < nVertices; i++)
        for(int j = 1; j <= Degree; j++)
            AddEdge(&graph, i, (i+j) % nVertices, rand() % 20);

    int src = 0;
    int* dist = (int*) malloc(sizeof(int) * nVertices);
    Dijkstra(&graph, src, dist);

    // Print the shortest distances stored in dist[]
    printf("Vertex Distances from Source (%d):\n", src);
    for(int i = 0; i < graph.nVertices; i++)
        printf("%d\t\t%d\n", i, dist[i]);
}
