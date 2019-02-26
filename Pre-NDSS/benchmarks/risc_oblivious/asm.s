	.file	"asm.c"
	.option nopic
	.text
	.align	2
	.globl	_cmovn
	.type	_cmovn, @function
_cmovn:
	blez	a3,.L1
	addiw	a5,a3,-1
	slli	a5,a5,32
	srli	a5,a5,32
	addi	a5,a5,1
	slli	a5,a5,2
	add	a5,a1,a5
.L3:
 #APP
# 85 "../include/asm.h" 1
	lw t1,(a1)
	lw t2,(a2)
	cmovw t2,a0,t1
	sw t2,(a2)
# 0 "" 2
 #NO_APP
	addi	a1,a1,4
	addi	a2,a2,4
	bne	a1,a5,.L3
.L1:
	ret
	.size	_cmovn, .-_cmovn
	.align	2
	.globl	_oswapn
	.type	_oswapn, @function
_oswapn:
	blez	a3,.L6
	addiw	a5,a3,-1
	slli	a5,a5,32
	srli	a5,a5,32
	addi	a5,a5,1
	slli	a5,a5,2
	add	a5,a1,a5
.L8:
 #APP
# 95 "../include/asm.h" 1
	lw t1,(a1)
	lw t2,(a2)
	mv t3,t2
	cmovw t2,a0,t1
	cmovw t1,a0,t3
	sw t1,(a1)
	sw t2,(a2)
# 0 "" 2
 #NO_APP
	addi	a1,a1,4
	addi	a2,a2,4
	bne	a1,a5,.L8
.L6:
	ret
	.size	_oswapn, .-_oswapn
	.ident	"GCC: (GNU) 7.2.0"
