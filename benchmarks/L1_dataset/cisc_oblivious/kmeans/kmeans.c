/***********************************
 * 
 * Copy-paste from wikicoding
 * 
 **********************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "../../../../primitives/lib/asm.h"
#include "../../../../primitives/cisc_insts/cisc.h"

static int seed = 0;
static int one = 1;
static float zero_float = 0.0;
static float max_val = 100.0;

static int dim = 2;
static int num_clusters = 32;
static int num_points = 1024;

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
int __attribute__((noinline)) Kmeans(float** data, float error, float** centroids, int* labels, int num_iters){

    
    int idx, pbase;

    float prev_total_dist = 0;
    float curr_total_dist = FLT_MAX;

    unsigned cluster_sizes = 0;
    unsigned temp_centroids = 0;
    sec_new(&cluster_sizes, num_clusters * sizeof(int));
    sec_new(&temp_centroids, num_clusters * dim * sizeof(float)); 

    /* Initialization */
    for(int i = 0; i < num_clusters; i++)
        for(int j = 0; j < dim; j++)
            centroids[i][j] = data[i][j];

    /* Main Loop */
    for(int h = 0; h < num_iters; h++){
        prev_total_dist = curr_total_dist;
        curr_total_dist = 0;

        // find the closest centroid to every point
        for(int i = 0; i < num_points; i++){
            float min_dist = FLT_MAX;
            for(int j = 0; j < num_clusters; j++){
                float dist = 0;
                for(int k = 0; k < dim; k++)
                   dist += pow(data[i][k] - centroids[j][k], 2);

                // if dist < min_dist, label[i] = j, min_dist = dist
                int cent_closer = (dist < min_dist);
                cmov(cent_closer, &j,          &labels[i]);
                cmov(cent_closer, (int*)&dist, (int*)&min_dist);
            }
            curr_total_dist += min_dist;
        }

        // clear out temp centroids
        for(int i = 0; i < dim; i++)
            for(int j = 0; j < num_clusters; j++){
                /*temp_centroids[i][j] = 0.0;*/
                idx = i * num_clusters + j;
                ocstore(zero_float, idx, temp_centroids);
            }

        // clear out current cluster sizes
        for(int i = 0; i < num_clusters; i++){
            /*cluster_sizes[i] = 0;*/
            ocstore(0, i, cluster_sizes);
        }

        // update temp centroid sum of destination cluster
        for(int i = 0; i < dim; i++){
            for(int j = 0; j < num_points; j++){
                // temp_centroids[i][labels[j]] += data[j][i];
                float centroid_val = 0;
                /*ScanORAM_Read((int*)temp_centroids[i], num_clusters, 1, (int*)&centroid_val, labels[j]);*/
                pbase = temp_centroids + i * num_clusters * sizeof(int);
                ocload(centroid_val, labels[j], pbase);
                centroid_val += data[j][i];
                /*ScanORAM_Write((int*)temp_centroids[i], num_clusters, 1, (int*)&centroid_val, labels[j]);*/
                ocstore(centroid_val, labels[j], pbase);
            }
        }
        // update current cluster sizes
        for(int i = 0; i < num_points; i++){
            // cluster_sizes[labels[i]]++;
            int cluster_size = 0;
            /*ScanORAM_Read(cluster_sizes, num_clusters, 1, &cluster_size, labels[i]);*/
            ocload(cluster_size, labels[i], cluster_sizes);
            cluster_size++;
            /*ScanORAM_Write(cluster_sizes, num_clusters, 1, &cluster_size, labels[i]);*/
            ocstore(cluster_size, labels[i], cluster_sizes);
        }

        // calculate the new centroids
        for(int i = 0; i < num_clusters; i++){
            for(int j = 0; j < dim; j++){
                // if cluster_sizes[i] == 0, to avoid divide_by_zero, we force cluster_size[i] = 1
                int cluster_sizes_i;
                ocload(cluster_sizes_i, i, cluster_sizes);
                cmov(!cluster_sizes_i, &one, &cluster_sizes_i);
                float temp_centroids_ji = 0;
                idx = j * num_clusters + i;
                ocload(temp_centroids_ji, idx, temp_centroids);
                centroids[i][j] = temp_centroids_ji / cluster_sizes_i;
            }
        }
    }

    sec_free(cluster_sizes, num_clusters * sizeof(int));
    sec_free(temp_centroids, num_clusters * dim * sizeof(float)); 

    if (fabs(curr_total_dist - prev_total_dist) > error)
        return 0;
    else
        return 1;
}


int main(){

    sec_init(L1_WAY_SIZE);
    srand(seed);

    int dim = 2;
    float error = 1e-4;
    int num_iters = 20;

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
    int converge = Kmeans(data_points, error, centroids, data_labels, num_iters);

    /*[> Print if converge <]*/
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

    sec_end();

    return 0;
}
