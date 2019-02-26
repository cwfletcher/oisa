#include <stdlib.h>
#include "scan_oram.h"
#include "../asm.h"


void ScanORAM_Read(int* base_addr, int N, int block_sz_w, int* data, int idx){
    for (int i=0; i<N; i++){
        int if_move = (i == idx);
        _cmovn(if_move, &base_addr[i*block_sz_w], data, block_sz_w);
    }
}

void ScanORAM_Write(int* base_addr, int N, int block_sz_w, int* data, int idx){
    for (int i=0; i<N; i++){
        int if_move = (i == idx);
        _cmovn(if_move, data, &base_addr[i*block_sz_w], block_sz_w);
    }
}
