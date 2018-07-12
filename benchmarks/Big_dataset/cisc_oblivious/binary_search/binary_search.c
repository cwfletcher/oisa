/************************************************************
 *
 *  Copy-paste from geeksforgeeks.org
 *
 ************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../../../primitives/lib/asm.h"
#include "../../../../primitives/lib/misc.h"
#include "../../../../primitives/cisc_insts/cisc.h"
#include "../../../../primitives/partpls_path_oram/path_oram.h"

static int N = (1 << 20);
static int seed = 0;
static int zero = 0;

int __attribute__((noinline)) BinarySearch(ORAM* oram, int l, int r, int x, int num_iter){
    int answer = -1;
    int arr_m = -1;
    for(int i = 0; i < num_iter; i++){
        int valid_op = (r >= l);
        int m = l + (r-l) / 2;

        // if r < l, m = 0
        cmov(!valid_op, &zero, &m);

        // read for arr[m]
        Access_ORAM(oram, READ, m, &arr_m);

        // if arr[m] == x, answer = m
        int match = (arr_m == x);
        cmov(match, &m, &answer);

        // if arr[m] < x, l = m+1
        int ignore_left = (arr_m < x);
        int m_plus1 = m + 1;
        cmov(ignore_left, &m_plus1, &l);

        // if arr[m] > x, r = m-1
        int ignore_right = (arr_m > x);
        int m_minus1 = m - 1;
        cmov(ignore_right, &m_minus1, &r);
    }

    // if we reach here, then element was not present
    return answer;
}

int main(){
    sec_init(L2_WAY_SIZE);

    ORAM oram;
    Init_ORAM(&oram, 4, N, 1, 200, BITONIC_SORT);

    for (int i = 0; i < N; i++)
        Write_Blocks(&oram, i, &i); 

    srand(seed);
    int val = rand() % (2*N);
    int num_iter = log_2_tight(N) + 1;
    sim_rdtsc();
    int result = BinarySearch(&oram, 0, N-1, val, num_iter);
    sim_rdtsc();

    (result == -1)? printf("Value %d is not in the array.\n", val) :
                    printf("Value %d is at index %d.\n", val, result);

    Free_ORAM(&oram);
    sec_end();
    return 0;
}
