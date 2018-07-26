#include <stdio.h>
#include <stdlib.h>

#include "include/asm.h"

int main() {
    int a = 1;
    int b = 2;
    int c = 3;
    int t1 = 0;
    int t2 = 0;

    _rdtsc(t1);
    c = a + b;
    _rdtsc(t2);

    printf("a=%d, b=%d, c=%d, t1=%d, t2=%d\n", a,b,c,t1,t2);


    _cmov(a, b, c);
    printf("a=%d, b=%d, c=%d\n", a,b,c);

}
