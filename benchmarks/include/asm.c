#include "asm.h"

extern inline void _cmovn(int if_mov, void* src_addr, void* dst_addr, int len);

extern inline void _oswapn(int if_swap, void* op1_addr, void* op2_addr, int len);
