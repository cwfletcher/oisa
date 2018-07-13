#include <stdio.h>
#include <stdlib.h>

static int N = 1024;
static int seed = 0;
static int zero = 0;

#define  _cmov(cond, src, dst) \
    __asm__ __volatile__ (  "lw t1,(%2)\n\t"                \
                            "cmovw t1,%0,%1\n\t"             \
                            "sw t1,(%2)"                    \
                            :                               \
                            : "r"(cond), "r"(src), "r"(dst) \
                            : "t1");  \

#define _rdtsc(counter) \
    __asm__ __volatile__ (  "csrrs %0, cycle, x0"   \
                            : "=r"(counter) );      \


int __attribute__ ((noinline)) BinarySearch(int* arr, int x){
    int answer = -1;
    for (int i = 0; i < N; i++){
        int match = (arr[i] == x);
        if (match != 0){
            printf("%d, %d, %d\n", match, i, answer);
        }
        _cmov(match, i, &answer);
        if (match != 0){
            printf("%d, %d, %d\n", match, i, answer);
        }
    }
    return answer;
}

int main(){
    int B = 1;
    int* arr = (int*) malloc(sizeof(int) * N * B);
    for(int i = 0; i < N*B; i++)
        arr[i] = i;

    srand(seed);
    int val = rand() % (2*N);
    int timer1;
    _rdtsc(timer1);
    int res = BinarySearch(arr, val);
    int timer2;
    _rdtsc(timer2);

    (res == -1)?    printf("Value %d is not in the array [0:%d]\n", val, N-1) :
                    printf("Value %d is at index %d in the array [0:%d]\n", val, res, N-1);
    printf("%d\n", timer2 - timer1);

    free(arr);
}
