#ifndef __MISC_HEADER__
#define __MISC_HEADER__

// calculate the log_2 value _after_ round val up to the closest power_of_2 value
// Example: log_2(1023) = 10, log_2(1024) = 10, log_2(1025) = 11
inline int log_2(int val){
    val = (val << 1) - 1;
    int result = 0;
    while (val>>=1)
        result ++;

    return result;
}

// calculate the log_2 value _after_ round val down to the closest power_of_2 value
// Example: log_2(1023) = 9, log_2(1024) = 10, log_2(1025) = 10
inline int log_2_tight(int val){
    int result = 0;
    while (val>>=1)
        result++;

    return result;
}

inline int bitwiseReverse(int val, int L){
    int result = 0;
    for (int i = 0; i < L; i++){
        result = (result << 1) + val % 2;
        val = val / 2;
    }
    return result;
}

inline int roundToPowerOf2(int val){
    int expt = 1;
    while (val>>=1)
        expt++;
    return (1 << expt);
}

#endif
