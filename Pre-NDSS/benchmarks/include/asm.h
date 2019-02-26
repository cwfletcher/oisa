/*
 * This file defines inline assembly primitives that used for benchmarks
 * These primitives are used for RISCV64 specifically
 *
 * Be careful that since primitives are defined in macro(not inline function in Multi2Sim version),
 * all parameters are passed as what they are wrote. Namely, if we pass pointer+1,
 * it is pointer value plus 1 instead of pointer value plus data type width.
 */
#ifndef __RISCV_ASM_HEADER__
#define __RISCV_ASM_HEADER__

#define _rdtsc(counter)                         \
    __asm__ __volatile__ (  "csrrs %0,cycle,x0" \
                            : "=r"(counter) );  \

#define _seal(reg)                              \
    __asm__ __volatile__ (  "seal %0,%0,0x0"    \
                            : "+r"(reg) );      \

#define _unseal(reg)                            \
    __asm__ __volatile__ (  "unseal %0,%0,0x0"  \
                            : "+r"(reg) );      \

#define _cmov(if_mov, src, dst_addr)                                \
    __asm__ __volatile__ (  "lw t1,(%2)\n\t"                        \
                            "cmovw t1,%0,%1\n\t"                    \
                            "sw t1,(%2)"                            \
                            :                                       \
                            : "r"(if_mov), "r"(src), "r"(dst_addr)  \
                            : "t1", "memory");                      \

#define _cmov_src_addr(if_mov, src_addr, dst_addr)                      \
    __asm__ __volatile__ (  "lw t1,(%1)\n\t"                            \
                            "lw t2,(%2)\n\t"                            \
                            "cmovw t2,%0,t1\n\t"                        \
                            "sw t2,(%2)"                                \
                            :                                           \
                            : "r"(if_mov), "r"(src_addr), "r"(dst_addr) \
                            : "t1", "t2", "memory");                    \

#define _oswap(if_swap, op1_addr, op2_addr)                                 \
    __asm__ __volatile__ (  "lw t1,(%1)\n\t"                                \
                            "lw t2,(%2)\n\t"                                \
                            "mv t3,t2\n\t"                                  \
                            "cmovw t2,%0,t1\n\t"                            \
                            "cmovw t1,%0,t3\n\t"                            \
                            "sw t1,(%1)\n\t"                                \
                            "sw t2,(%2)"                                    \
                            :                                               \
                            : "r"(if_swap), "r"(op1_addr), "r"(op2_addr)    \
                            : "t1", "t2", "t3", "memory");                  \

inline void _cmovn(int if_mov, void* src_addr, void* dst_addr, int len) {
    void* curr_src_addr = src_addr;
    void* curr_dst_addr = dst_addr;
    for(int i = 0; i < len; i++){
        _cmov_src_addr(if_mov, curr_src_addr, curr_dst_addr);
        curr_src_addr += sizeof(int);
        curr_dst_addr += sizeof(int);
    }
}

inline void _oswapn(int if_swap, void* op1_addr, void* op2_addr, int len){
    void* curr_op1_addr = op1_addr;
    void* curr_op2_addr = op2_addr;
    for(int i = 0; i < len; i++){
        _oswap(if_swap, curr_op1_addr, curr_op2_addr);
        curr_op1_addr += sizeof(int);
        curr_op2_addr += sizeof(int);
    }
}

/* Backup functions
inline void _rdtsc(counter){
    __asm__ __volatile__ (  "csrrs %0,cycle,x0"
                            : "=r"(counter) );
}

inline void _cmov(if_mov, src, dst_addr){
    __asm__ __volatile__ (  "lw t1,(%2)\n\t"
                            "cmovw t1,%0,%1\n\t"
                            "sw t1,(%2)"
                            :
                            : "r"(if_mov), "r"(src), "r"(dst_addr)
                            : "t1");
}

inline void _cmov_src_addr(int if_mov, void* src_addr, void* dst_addr){
    __asm__ __volatile__ (  "lw t1,(%1)\n\t"
                            "lw t2,(%2)\n\t"
                            "cmovw t2,%0,t1\n\t"
                            "sw t2,(%2)"
                            :
                            : "r"(if_mov), "r"(src_addr), "r"(dst_addr)
                            : "t1", "t2");
}

inline void _oswap(int if_swap, void* op1_addr, void* op2_addr){                                 \
    __asm__ __volatile__ (  "lw t1,(%1)\n\t"
                            "lw t2,(%2)\n\t"
                            "mv t3,t2\n\t"
                            "cmovw t2,%0,t1\n\t"
                            "cmovw t1,%0,t3\n\t"
                            "sw t1,(%1)\n\t"
                            "sw t2,(%2)"
                            :
                            : "r"(if_swap), "r"(op1_addr), "r"(op2_addr)
                            : "t1", "t2", "t3");
}
*/
#endif
