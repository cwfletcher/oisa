#include <stdio.h>
#include <stdlib.h>

#include "include/asm.h"

int main(){
    int a = 1;
    int b = 2;
    _seal(a);
    int c = a + b;
    if(c)
        printf("Pass\n");
    _unseal(c);
    /*if(c)*/
        /*printf("Pass\n");*/
}
