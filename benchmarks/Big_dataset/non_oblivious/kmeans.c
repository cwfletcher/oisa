/***********************************
 * 
 * Copy-paste from wikicoding
 * 
 **********************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "../../../primitives/lib/asm.h"

static int seed = 0;
static float max_val = 100.0;

static int dim = 2;
static int num_clusters = (1 << 8);
static int num_points = (1 << 14);

float distance(float* p1, float* p2, int dim){
    float dist = 0.0;
    for(int i = 0; i < dim; i++)
        dist += pow(p1[i] - p2[i], 2);
    return sqrt(dist);
}

// A simple k-means clustering routine
// Return True  if final sum of distances < error before reaches num_iters
//        False if final sum of distances > error when reaches num_iters
// Parameters:  float** data:      array of data pointes
//              int num_points:     number of data points
//              int dim:            number of dimensions
//              int num_clusters:   desired number of clusters
//              float error:       used as the stopping criterion
//              float** centroids: output address of the final centroids
//              int* labels:        cluster labels of all the data points
int __attribute__((noinline)) Kmeans(float** data, float error, float** centroids, int* labels){

    float prev_total_dist = 0;
    float curr_total_dist = FLT_MAX;

    int* cluster_sizes = (int*) calloc(num_clusters, sizeof(int));

    /* Initialization */
    for(int i = 0; i < num_clusters; i++)
        for(int j = 0; j < dim; j++)
            centroids[i][j] = data[i][j];

    /* Main Loop */
    /*while(fabs(curr_total_dist - prev_total_dist) > error){*/
        /*prev_total_dist = curr_total_dist;*/
        /*curr_total_dist = 0;*/

        
        /*// find the closest centroid to every point*/
        /*for(int i = 0; i < num_points; i++){*/
            /*float min_dist = FLT_MAX;*/
            /*for(int j = 0; j < num_clusters; j++){*/
                /*float dist = 0;*/
                /*for(int k = 0; k < dim; k++)*/
                   /*dist += pow(data[i][k] - centroids[j][k], 2);*/
                /*if (dist < min_dist){*/
                    /*labels[i] = j;*/
                    /*min_dist = dist;*/
                /*}*/
            /*}*/
            /*curr_total_dist += min_dist;*/
        /*}*/

        /*// clear out current counts and current centroids*/
        /*for(int i = 0; i < num_clusters; i++){*/
            /*cluster_sizes[i] = 0;*/
            /*for(int j = 0; j < dim; j++)*/
                /*centroids[i][j] = 0.0;*/
        /*}*/

        /*// update size and temp centroid of destination cluster*/
        /*for(int i = 0; i < num_points; i++){*/
            /*cluster_sizes[labels[i]]++;*/
            /*for(int j = 0; j < dim; j++){*/
                /*centroids[labels[i]][j] += data[i][j];*/
            /*} */
        /*} */
        /*for(int i = 0; i < num_clusters; i++){*/
            /*for(int j = 0; j < dim; j++){*/
                /*centroids[i][j] = cluster_sizes[i] ? centroids[i][j] / cluster_sizes[i] */
                                                   /*: centroids[i][j];*/
            /*}*/
        /*}*/
    /*}*/

    free(cluster_sizes);

    return 1;
}


int main(){

    srand(seed);

    float error = 1e-4;

    /* Initialize all data points */
    float** data_points = (float**) malloc(sizeof(float*) * num_points);
    for(int i = 0; i < num_points; i++){
        data_points[i] = (float*) malloc(sizeof(float) * dim);
        for(int j = 0; j < dim; j++)
            data_points[i][j] = (float)rand() / RAND_MAX * max_val;
    }

    /* Initialize all centroids */
    float** centroids = (float**) malloc(sizeof(float*) * num_clusters);
    for(int i = 0; i < num_clusters; i++)
        centroids[i] = (float*) malloc(sizeof(float) * dim);

    /* Initialize all data labels */
    int* data_labels = (int*) malloc(sizeof(int) * num_points);

    /* Run k-means */
    sim_rdtsc();
    int converge = Kmeans(data_points, error, centroids, data_labels);
    sim_rdtsc();

    /* Print if converge */
    if (converge)
        printf("Below error\n");
    else
        printf("Above error\n");

    /*[> Print all centroids <]*/
    /*for(int i = 0; i < num_clusters; i++){*/
        /*printf("centroid %d is (", i);*/
        /*for(int j = 0; j < dim; j++)*/
            /*printf("%f,", centroids[i][j]);*/
        /*printf(")\n");*/
    /*}*/

    /*[> Print all data labels <]*/
    /*for(int i = 0; i < num_points; i++){*/
        /*printf("data point %d (", i);*/
        /*for(int j = 0; j < dim; j++)*/
            /*printf("%f,", data_points[i][j]);*/
        /*printf(") is in cluster %d; distance = %f\n", data_labels[i], distance(data_points[i], centroids[data_labels[i]], dim));*/
    /*}*/

    /* Free data */
    for(int i = 0; i < num_points; i++)
        free(data_points[i]);
    free(data_points);
    for(int i = 0; i < num_clusters; i++)
        free(centroids[i]);
    free(centroids);
    free(data_labels);

    return 0;
}
