/*
 *  Oblivious DNN is the same as non-oblivious DNN.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ReLU(x) (x > 0) ? x : 0
static int seed = 0;
static int nInputs = 8 * 1024;
static int nL1Neurons = 32 * 1024;
static int nOutputs = 512;

float GenRandFloat(int max){
    return ((float)rand() / (float)(RAND_MAX / max));
}

void __attribute__((noinline)) Inference(float* input, float* output, float* W1, float* B1, float* W2, float* B2){
    float* L1Output = (float*) malloc(sizeof(float) * nL1Neurons);
    float* L2Output = (float*) malloc(sizeof(float) * nOutputs);

    /// L1Output = ReLU(W1 * input + B2)
    for(int i = 0; i < nL1Neurons; i++){
        L1Output[i] = B1[i];
        for(int j = 0; j < nInputs; j++)
            L1Output[i] += W1[i * nInputs + j] * input[j];
        L1Output[i] = ReLU(L1Output[i]);
    }

    /// L2Output = ReLU(W2 * L1Output + B1)
    for(int i = 0; i < nOutputs; i++){
        L2Output[i] = B2[i];
        for(int j = 0; j < nL1Neurons; j++)
            L2Output[i] += W2[i * nL1Neurons + j] * L1Output[j];
        L2Output[i] = ReLU(L2Output[i]);
    }

    /// output = SoftMax(L2Output)
    float sum = 0.0;
    for(int i = 0; i < nOutputs; i++){
        L2Output[i] = exp(L2Output[i]);
        sum += L2Output[i];
    }

    for(int i = 0; i < nOutputs; i++){
        output[i] = L2Output[i] / sum;
    }

    free(L1Output);
    free(L2Output);
}

int main(){
    srand(seed);

    // Initialize inputs
    float* input = (float*) malloc(sizeof(float) * nInputs);
    for(int i = 0; i < nInputs; i++)
        input[i] = GenRandFloat(3);

    // Initialize outputs
    float* output = (float*) malloc(sizeof(float) * nOutputs);

    // Initialize layer 1
    float* W1 = (float*) malloc(sizeof(float) * nInputs * nL1Neurons);
    float* B1 = (float*) malloc(sizeof(float) * nL1Neurons);
    for(int i = 0; i < nL1Neurons; i++){
        B1[i] = GenRandFloat(1) - GenRandFloat(1);
        for(int j = 0; j < nInputs; j++)
            W1[i*nInputs + j] = GenRandFloat(1) - GenRandFloat(1);
    }

    // Initialize layer 2
    float* W2 = (float*) malloc(sizeof(float) * nL1Neurons * nOutputs);
    float* B2 = (float*) malloc(sizeof(float) * nOutputs);
    for(int i = 0; i < nOutputs; i++){
        B2[i] = GenRandFloat(1) - GenRandFloat(1);
        for(int j = 0; j < nL1Neurons; j++)
            W2[i*nL1Neurons + j] = GenRandFloat(1) - GenRandFloat(1);
    }

    Inference(input, output, W1, B1, W2, B2);

    // Print output
    /*for(int i = 0; i < nOutputs; i++)*/
        /*printf("%f ", output[i]);*/
    /*printf("\n");*/

    free(input);
    free(output);
    free(W1);
    free(B1);
    free(W2);
    free(B2);

    return 0;
}
