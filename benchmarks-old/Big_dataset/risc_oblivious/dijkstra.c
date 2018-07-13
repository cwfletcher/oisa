/*
 *  Sparse Dijkstra algorithm using loop coalescing
 *  Copy-paste from ObliVM paper.
 *
 *  For sparse graph,
 *      sizeof PQ        = O(E) = O(V) (use scan_oram)
 *      sizeof dist      = O(V)        (use scan_oram)
 *      sizeof edges     = O(E) = O(V) (use scan_oram)
 *      sizeof sumDegree = O(V)    (use scan_oram)
 */

#include <limits.h>
#include "PQ.h"
#include "../../../primitives/lib/asm.h"
#include "../../../primitives/sort/sort.h"
#include "../../../primitives/scan_oram/scan_oram.h"

static int seed = 0;
static int zero = 0;
static int Degree = 4;
static int nVertices = (1 << 12) - 1;
static int nEdges = ((1 << 12) - 1) * 4; // nVertices * Degree;


typedef struct {
    int   nAddedEdges;
    int*  nOutDegrees;
    int*  nOutDegreesAcc;
    int*  edges;
} Graph;

void Init_Graph(Graph* graph){
    graph->nAddedEdges    = 0;
    graph->nOutDegrees    = (int*) malloc(sizeof(int) * nVertices);
    graph->nOutDegreesAcc = (int*) malloc(sizeof(int) * (nVertices+1));
    graph->edges          = (int*) malloc(sizeof(int) * 3 * nEdges * 2);

}

void Print_Graph(Graph* graph){
    for(int i = 0; i < nVertices; i++)
        printf("vertex %d (degree = %d): \n", i, graph->nOutDegrees[i]);

    for(int i = 0; i < nEdges * 2; i++)
        printf("edge: %d -> %d, weight = %d\n", graph->edges[i*3], graph->edges[i*3+1], graph->edges[i*3+2]);
}

int less_than(int* a, int *b){
    return (a[1] > b[1]);
}

/// Oblivious Dijkstra's algorithm
void __attribute__((noinline)) Dijkstra(Graph* graph, int src, int* dists){

    for(int i = 0; i < nVertices; i++)
        dists[i] = INT_MAX;
    dists[src] = 0;

    PQ pq;
    Init_PQ(&pq, nVertices, 2);
    int src_node [] = {src, 0}; // dist = 0, node = src
    Push_PQ(&pq, src_node, 1);

    int innerLoop = 0;

    int node[2] = {0,0};
    int edge[3] = {0,0,0};
    int u = src;
    int v = src;
    int dist = 0;
    int newdist = 0;
    int i = 0;
    /*for(int h = 0; h < 2 * nVertices + 2 * nEdges + nVertices/2; h++){*/
    for(int h = 0; h < 8; h++){

        int innerLoop_True = innerLoop;
        int innerLoop_False = !innerLoop;

        /// If not innerLoop
        Top_PQ(&pq, node);
        Pop_PQ(&pq, innerLoop_False);
        cmov(innerLoop_False, &node[0], &u);
        cmov(innerLoop_False, &node[1], &dist);

        int dist_u = 0;
        ScanORAM_Read(dists, nVertices, 1, &dist_u, u);

        ///     If dist[u] == dist
        int dist_u_eq_dist = (dist_u == dist);
        int nOutDegreesAcc_u = 0;
        ScanORAM_Read(graph->nOutDegreesAcc, nVertices+1, 1, &nOutDegreesAcc_u, u);
        cmov(innerLoop_False && dist_u_eq_dist, &nOutDegreesAcc_u, &i);              // i = 0
        cmov(innerLoop_False && dist_u_eq_dist, &one, &innerLoop) ;      // innerLoop = True

        /// Else
        //      If i < nOutDegrees[u]
        int nOutDegreesAcc_up1 = 0;
        ScanORAM_Read(graph->nOutDegreesAcc, nVertices+1, 1, &nOutDegreesAcc_up1, u+1);
        int i_lt_nOutDegreesAcc_up1 = (i < nOutDegreesAcc_up1);
        ScanORAM_Read(graph->edges, nEdges*2, 3, edge, i);
        v = edge[1];
        int dist_p_w = dist + edge[2];
        cmov(innerLoop_True && i_lt_nOutDegreesAcc_up1, &dist_p_w, &newdist);
        int ip1 = i+1;
        cmov(innerLoop_True && i_lt_nOutDegreesAcc_up1, &ip1, &i);

        //          If newdist < dis_v
        int dist_v = 0;
        ScanORAM_Read(dists, nVertices, 1, &dist_v, v);
        int newdist_lt_dist_v = (newdist < dist_v);
        cmov(innerLoop_True && i_lt_nOutDegreesAcc_up1 && newdist_lt_dist_v, &newdist, &dist_v);
        ScanORAM_Write(dists, nVertices, 1, &dist_v, v);
        node[0] = v;
        node[1] = newdist;
        Push_PQ(&pq, node, innerLoop_True && i_lt_nOutDegreesAcc_up1 && newdist_lt_dist_v);

        //      Else
        cmov(innerLoop_True && !i_lt_nOutDegreesAcc_up1, &zero, &innerLoop);
    }

    Delete_PQ(&pq);
}

void AddEdge(Graph* graph, int u, int v, int weight){
    assert (u != v);
    assert (graph->nAddedEdges < nEdges);
    // add an edge from u to v
    graph->edges[graph->nAddedEdges * 6] = u;
    graph->edges[graph->nAddedEdges * 6 + 1] = v;
    graph->edges[graph->nAddedEdges * 6 + 2] = weight;
    graph->nOutDegrees[u]++;

    // add an edge from v to u
    graph->edges[graph->nAddedEdges * 6 + 3] = v;
    graph->edges[graph->nAddedEdges * 6 + 4] = u;
    graph->edges[graph->nAddedEdges * 6 + 5] = weight;
    graph->nOutDegrees[v]++;

    graph->nAddedEdges++;
}

void OrganizeEdges(Graph* graph){
    graph->nOutDegreesAcc[0] = 0;
    assert (graph->nAddedEdges == nEdges);
    for(int i = 0; i < nVertices; i++){
        graph->nOutDegreesAcc[i+1] = graph->nOutDegrees[i] + graph->nOutDegreesAcc[i];
    }
    MergeSort_Block(graph->edges, nEdges*2, 0, 1, 3);
}

int main(){

    srand(seed);

    Graph graph;
    // create the graph
    Init_Graph(&graph);

    // makie graph
    for(int i = 0; i < nVertices; i++)
        for(int j = 1; j <= Degree; j++)
            AddEdge(&graph, i, (i+j) % nVertices, rand() % 20);

    OrganizeEdges(&graph);
    /*Print_Graph(&graph);*/

    int src = 0;
    int* dists = (int*) malloc(sizeof(int) * nVertices);
    sim_rdtsc();
    Dijkstra(&graph, src, dists);
    sim_rdtsc();

    // Print the shortest distances stored in dists[]
    /*printf("Vertex Distances from Source (%d):\n", src);*/
    /*for(int i = 0; i < nVertices; i++)*/
        /*printf("%d\t\t%d\n", i, dists[i]);*/
}
