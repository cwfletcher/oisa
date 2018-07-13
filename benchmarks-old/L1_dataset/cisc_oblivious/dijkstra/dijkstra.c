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

static int seed = 0;
static int zero = 0;
static int int_max = INT_MAX;
static int Degree = 4;
static int nVertices = (1 << 7) - 1;
static int nEdges = ((1 << 7) - 1) * 4; // nVertices * Degree;


typedef struct {
    int      nAddedEdges;
    int*     nOutDegrees;
    int*     edges_in_virt;
    unsigned nOutDegreesAcc;
    unsigned edges;
} Graph;

void Init_Graph(Graph* graph){
    graph->nAddedEdges    = 0;
    graph->nOutDegrees    = (int*) malloc(sizeof(int) * nVertices);
    graph->edges_in_virt  = (int*) malloc(sizeof(int) * 3 * nEdges * 2);
    /*graph->nOutDegreesAcc = (int*) malloc(sizeof(int) * (nVertices+1));*/
    sec_new(&graph->nOutDegreesAcc, sizeof(int) * (nVertices + 1));
    sec_new(&graph->edges, sizeof(int) * 3 * nEdges * 2);

}

void Print_Graph(Graph* graph){
    for(int i = 0; i < nVertices; i++)
        printf("vertex %d (degree = %d): \n", i, graph->nOutDegrees[i]);

    int idx = 0;
    for(int i = 0; i < nEdges * 2; i++){
        int u = 0, v = 0, w = 0;
        idx = 3 * i;
        ocload(u, idx, graph->edges);
        idx++;
        ocload(v, idx, graph->edges);
        idx++;
        ocload(w, idx, graph->edges);
        printf("edge: %d -> %d, weight = %d\n", u, v, w);
    }
}

int less_than(unsigned a, unsigned b){
    int va, vb;
    ocload(va, 1, a);
    ocload(vb, 1, b);
    return (va > vb);
}

/// Oblivious Dijkstra's algorithm
void __attribute__((noinline)) Dijkstra(Graph* graph, int src){

    unsigned dists;     // a pls region of nVertices words
    sec_new(&dists, sizeof(int) * nVertices);
    for(int i = 0; i < nVertices; i++)
        // dists[i] = INT_MAX;
        ocstore(int_max, i, dists);
    /*dists[src] = 0;*/
    ocstore(0, src, dists);

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
    int idx = 0;

    for(int h = 0; h < 2 * nVertices + 2 * nEdges + nVertices/2; h++){

        int innerLoop_True = innerLoop;
        int innerLoop_False = !innerLoop;

        /// If not innerLoop
        Top_PQ(&pq, node);
        Pop_PQ(&pq, innerLoop_False);
        cmov(innerLoop_False, &node[0], &u);
        cmov(innerLoop_False, &node[1], &dist);

        int dist_u = 0;
        ocload(dist_u, u, dists);

        ///     If dist[u] == dist
        int dist_u_eq_dist = (dist_u == dist);
        int nOutDegreesAcc_u = 0;
        ocload(nOutDegreesAcc_u, u, graph->nOutDegreesAcc);
        cmov(innerLoop_False && dist_u_eq_dist, &nOutDegreesAcc_u, &i);              // i = 0
        cmov(innerLoop_False && dist_u_eq_dist, &one, &innerLoop) ;      // innerLoop = True

        /// Else
        //      If i < nOutDegrees[u]
        int nOutDegreesAcc_up1 = 0;
        idx = u + 1;
        ocload(nOutDegreesAcc_up1, idx, graph->nOutDegreesAcc);
        int i_lt_nOutDegreesAcc_up1 = (i < nOutDegreesAcc_up1);

        idx = i * 3;
        ocload(edge[0], idx, graph->edges);
        idx++;
        ocload(edge[1], idx, graph->edges);
        idx++;
        ocload(edge[2], idx, graph->edges);

        v = edge[1];
        int dist_p_w = dist + edge[2];
        cmov(innerLoop_True && i_lt_nOutDegreesAcc_up1, &dist_p_w, &newdist);
        int ip1 = i+1;
        cmov(innerLoop_True && i_lt_nOutDegreesAcc_up1, &ip1, &i);
            
        //          If newdist < dis_v
        int dist_v = 0;
        ocload(dist_v, v, dists);
        int newdist_lt_dist_v = (newdist < dist_v);
        cmov(innerLoop_True && i_lt_nOutDegreesAcc_up1 && newdist_lt_dist_v, &newdist, &dist_v);
        ocstore(dist_v, v, dists);
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
    ocstore(0, 0, graph->nOutDegreesAcc);
    assert (graph->nAddedEdges == nEdges);
    int outDegreesAcc_i;
    int ip1 = 0;
    for(int i = 0; i < nVertices; i++){
        ocload(outDegreesAcc_i, i, graph->nOutDegreesAcc);
        outDegreesAcc_i += graph->nOutDegrees[i];
        ip1 = i + 1;
        ocstore(outDegreesAcc_i, ip1, graph->nOutDegreesAcc);
        /*graph->nOutDegreesAcc[i+1] = graph->nOutDegrees[i] + graph->nOutDegreesAcc[i];*/
    }

    /// Sort edges in the order of source
    MergeSort_Block(graph->edges_in_virt, nEdges*2, 0, 1, 3);
    memcpy_int_in_pls(graph->edges_in_virt, graph->edges, 3 * 2 * nEdges);
}

int main(){

    srand(seed);
    sec_init(4 * L1_WAY_SIZE);

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
