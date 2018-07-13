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
#include "../../../../primitives/lib/asm.h"
#include "../../../../primitives/sort/sort.h"
#include "../../../../primitives/cisc_insts/cisc.h"
#include "../../../../primitives/partpls_path_oram/path_oram.h"

static int seed = 0;
static int zero = 0;
static int int_max = INT_MAX;
static int Degree = 4;
static int nVertices = (1 << 12) - 1;
static int nEdges = ((1 << 12) - 1) * 4; // nVertices * Degree;


typedef struct {
    int     nAddedEdges;
    int*    nOutDegrees;
    int*    edges_in_virt;
    ORAM    nOutDegreesAcc;
    ORAM    edges;
} Graph;

void Init_Graph(Graph* graph){
    graph->nAddedEdges    = 0;
    graph->nOutDegrees    = (int*) malloc(sizeof(int) * nVertices);
    graph->edges_in_virt  = (int*) malloc(sizeof(int) * 3 * nEdges * 2);
    /*graph->nOutDegreesAcc = (int*) malloc(sizeof(int) * (nVertices+1));*/
    Init_ORAM(&graph->nOutDegreesAcc, 4, (nVertices+1), 1, 200, BITONIC_SORT);
    Init_ORAM(&graph->edges, 4, (nEdges*2), 3, 200, BITONIC_SORT);

}

void Print_Graph(Graph* graph){
    for(int i = 0; i < nVertices; i++)
        printf("vertex %d (degree = %d) \n", i, graph->nOutDegrees[i]);

    for(int i = 0; i < nEdges * 2; i++){
        int edge[3] = {0,0,0};
        Access_ORAM(&graph->edges, READ, i, edge);
        printf("edge: %d -> %d, weight = %d\n", edge[0], edge[1], edge[2]);
    }
}

int less_than(int* a, int *b){
    return (a[1] > b[1]);
}

/// Oblivious Dijkstra's algorithm
void __attribute__((noinline)) Dijkstra(Graph* graph, int src){

    ORAM dists;
    Init_ORAM(&dists, 4, nVertices, 1, 200, BITONIC_SORT);
    for(int i = 0; i < nVertices; i++)
        Write_Blocks(&dists, i, &int_max);
    Write_Blocks(&dists, src, &zero);

    PQ pq;  // a pls region of nVertices x 2 words
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
    /*for(int h = 0; h < 2 * nVertices + 2 * nEdges + nVertices / 2; h++){*/
    for(int h = 0; h < 8; h++){

        int innerLoop_True = innerLoop;
        int innerLoop_False = !innerLoop;

        /// If not innerLoop
        Top_PQ(&pq, node);
        printf("top\n");
        Pop_PQ(&pq, innerLoop_False);
        printf("pop\n");
        cmov(innerLoop_False, &node[0], &u);
        cmov(innerLoop_False, &node[1], &dist);

        int dist_u = 0;
        /*ocload(dist_u, u, dists);*/
        Access_ORAM(&dists, READ, u, &dist_u);
        printf("access 1\n");

        ///     If dist[u] == dist
        int dist_u_eq_dist = (dist_u == dist);
        int nOutDegreesAcc_u = 0;
        /*ocload(nOutDegreesAcc_u, u, graph->nOutDegreesAcc);*/
        Access_ORAM(&graph->nOutDegreesAcc, READ, u, &nOutDegreesAcc_u);
        printf("access 2\n");
        cmov(innerLoop_False && dist_u_eq_dist, &nOutDegreesAcc_u, &i);              // i = 0
        cmov(innerLoop_False && dist_u_eq_dist, &one, &innerLoop) ;      // innerLoop = True

        /// Else
        //      If i < nOutDegrees[u]
        int nOutDegreesAcc_up1 = 0;
        /*ocload(nOutDegreesAcc_up1, idx, graph->nOutDegreesAcc);*/
        Access_ORAM(&graph->nOutDegreesAcc, READ, u+1, &nOutDegreesAcc_up1);
        printf("access 3\n");
        int i_lt_nOutDegreesAcc_up1 = (i < nOutDegreesAcc_up1);

        Access_ORAM(&graph->edges, READ, i, edge);
        printf("access 4\n");

        v = edge[1];
        int dist_p_w = dist + edge[2];
        cmov(innerLoop_True && i_lt_nOutDegreesAcc_up1, &dist_p_w, &newdist);
        int ip1 = i+1;
        cmov(innerLoop_True && i_lt_nOutDegreesAcc_up1, &ip1, &i);

        //          If newdist < dis_v
        int dist_v = 0;
        /*ocload(dist_v, v, dists);*/
        Access_ORAM(&dists, READ, v, &dist_v);
        printf("access 5\n");
        int newdist_lt_dist_v = (newdist < dist_v);
        cmov(innerLoop_True && i_lt_nOutDegreesAcc_up1 && newdist_lt_dist_v, &newdist, &dist_v);
        /*ocstore(dist_v, v, dists);*/
        Access_ORAM(&dists, WRITE, v, &dist_v);
        printf("access 6\n");
        node[0] = v;
        node[1] = newdist;
        Push_PQ(&pq, node, innerLoop_True && i_lt_nOutDegreesAcc_up1 && newdist_lt_dist_v);

        //      Else
        cmov(innerLoop_True && !i_lt_nOutDegreesAcc_up1, &zero, &innerLoop);
        if (h % nVertices)
            printf("%d\n", h);
    }

    Delete_PQ(&pq);
}

void AddEdge(Graph* graph, int u, int v, int weight){
    assert (u != v);
    assert (graph->nAddedEdges < nEdges);
    // add an edge from u to v
    graph->edges_in_virt[graph->nAddedEdges * 6] = u;
    graph->edges_in_virt[graph->nAddedEdges * 6 + 1] = v;
    graph->edges_in_virt[graph->nAddedEdges * 6 + 2] = weight;
    graph->nOutDegrees[u]++;

    // add an edge from v to u
    graph->edges_in_virt[graph->nAddedEdges * 6 + 3] = v;
    graph->edges_in_virt[graph->nAddedEdges * 6 + 4] = u;
    graph->edges_in_virt[graph->nAddedEdges * 6 + 5] = weight;
    graph->nOutDegrees[v]++;

    graph->nAddedEdges++;
}

void OrganizeEdges(Graph* graph){

    /// Calculate outdegreeacc
    /*graph->nOutDegreesAcc[0] = 0;*/
    Write_Blocks(&graph->nOutDegreesAcc, 0, &zero);
    assert (graph->nAddedEdges == nEdges);
    int outDegreesAcc_i = 0;
    for(int i = 0; i < nVertices; i++){
        outDegreesAcc_i += graph->nOutDegrees[i];
        /*graph->nOutDegreesAcc[i+1] = graph->nOutDegrees[i] + graph->nOutDegreesAcc[i];*/
        Write_Blocks(&graph->nOutDegreesAcc, i+1, &outDegreesAcc_i);
    }

    /// Sort edges in the order of source
    MergeSort_Block(graph->edges_in_virt, nEdges*2, 0, 1, 3);
    for(int i = 0; i < nEdges * 2; i++){
        int edge[3];
        edge[0] = graph->edges_in_virt[3*i];
        edge[1] = graph->edges_in_virt[3*i+1];
        edge[2] = graph->edges_in_virt[3*i+2];
        Write_Blocks(&graph->edges, i, edge);
    }
}

int main(){

    srand(seed);
    sec_init(2 * L2_WAY_SIZE);

    Graph graph;
    // create the graph
    Init_Graph(&graph);

    // makie graph
    for(int i = 0; i < nVertices; i++)
        for(int j = 1; j <= Degree; j++)
            AddEdge(&graph, i, (i+j) % nVertices, rand() % 20);

    OrganizeEdges(&graph);

    int src = 0;
    sim_rdtsc();
    Dijkstra(&graph, src);
    sim_rdtsc();

    sec_end();

    // Print the shortest distances stored in dists[]
    /*printf("Vertex Distances from Source (%d):\n", src);*/
    /*for(int i = 0; i < nVertices; i++)*/
        /*printf("%d\t\t%d\n", i, dists[i]);*/

    return 0;
}
