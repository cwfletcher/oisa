/*
 *  Non-oblivious DNN is the same as oblivious DNN.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../primitives/lib/asm.h"

#define ReLU(x) (x > 0) ? x : 0
static int seed = 0;
static int nInputs = 8 * 1024;
static int nL1Neurons = 32 * 1024;
static int nOutputs = 512;

static float* input;
static float* output;
static float* W1;
static float* B1;
static float* W2;
static float* B2;
static float* L1Output;
static float* L2Output;


float GenRandFloat(int max){
    return ((float)rand() / (float)(RAND_MAX / max));
}

void __attribute__((noinline)) Layer1(){
    /// L1Output = ReLU(W1 * input + B2)
    /*for(int i = 0; i < nL1Neurons; i++){*/
    for(int i = 0; i < 1024; i++){
        L1Output[i] = B1[i];
        for(int j = 0; j < nInputs; j++)
            L1Output[i] += W1[i * nInputs + j] * input[j];
        L1Output[i] = ReLU(L1Output[i]);
    }
}
void __attribute__((noinline)) Layer2(){
    /// L2Output = ReLU(W2 * L1Output + B1)
    /*for(int i = 0; i < nOutputs; i++){*/
    for(int i = 0; i < 16; i++){
        L2Output[i] = B2[i];
        for(int j = 0; j < nL1Neurons; j++)
            L2Output[i] += W2[i * nL1Neurons + j] * L1Output[j];
        L2Output[i] = ReLU(L2Output[i]);
    }
}
void __attribute__((noinline)) Output(){
    /// output = SoftMax(L2Output)
    float sum = 0.0;
    /*for(int i = 0; i < nOutputs; i++){*/
    for(int i = 0; i < 16; i++){
        L2Output[i] = exp(L2Output[i]);
        sum += L2Output[i];
    }

    /*for(int i = 0; i < nOutputs; i++){*/
    for(int i = 0; i < 16; i++){
        output[i] = L2Output[i] / sum;
    }
}


int main(){
    srand(seed);

    // Initialize inputs
    input = (float*) malloc(sizeof(float) * nInputs);

    // Initialize outputs
    output = (float*) malloc(sizeof(float) * nOutputs);

    // Initialize layer 1
    W1 = (float*) malloc(sizeof(float) * nInputs * nL1Neurons);
    B1 = (float*) malloc(sizeof(float) * nL1Neurons);

    // Initialize layer 2
    W2 = (float*) malloc(sizeof(float) * nL1Neurons * nOutputs);
    B2 = (float*) malloc(sizeof(float) * nOutputs);

    L1Output = (float*) malloc(sizeof(float) * nL1Neurons);
    L2Output = (float*) malloc(sizeof(float) * nOutputs);

    sim_rdtsc();
    Layer1();
    sim_rdtsc();
    Layer2();
    sim_rdtsc();
    Output();
    sim_rdtsc();

    // Print output
    /*for(int i = 0; i < nOutputs; i++)*/
        /*printf("%f ", output[i]);*/
    /*printf("\n");*/

    free(L1Output);
    free(L2Output);
    free(input);
    free(output);
    free(W1);
    free(B1);
    free(W2);
    free(B2);

    return 0;
}
