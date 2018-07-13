#include "misc.h"

int log_2(int val){
    val = (val << 1) - 1;
    int result = 0;
    while (val>>=1)
        result ++;

    return result;
}

int log_2_tight(int val){
    int result = 0;
    while (val>>=1)
        result++;

    return result;
}

int bitwiseReverse(int val, int L){
    int result = 0;
    for (int i = 0; i < L; i++){
        result = (result << 1) + val % 2;
        val = val / 2;
    }
    return result;
}

int roundToPowerOf2(int val){
    int expt = 1;
    while (val>>=1)
        expt++;
    return (1 << expt);
}
