	.file	"binary_search.c"
	.option nopic
	.text
	.align	2
	.globl	BinarySearch
	.type	BinarySearch, @function
BinarySearch:
	addi	sp,sp,-80
	li	a5,-1
	sd	s0,64(sp)
	sd	s1,56(sp)
	sd	s2,48(sp)
	sd	s3,40(sp)
	sd	s4,32(sp)
	sd	s5,24(sp)
	sd	s6,16(sp)
	sd	ra,72(sp)
	mv	s3,a1
	sw	a5,12(sp)
	mv	s1,a0
	li	s0,0
	li	s6,1
	addi	s5,sp,12
	lui	s4,%hi(.LC0)
	li	s2,1024
	j	.L3
.L2:
	addiw	s0,s0,1
	addi	s1,s1,4
	beq	s0,s2,.L7
.L3:
	lw	a5,0(s1)
	bne	a5,s3,.L2
 #APP
# 19 "binary_search.c" 1
	lw t1,(s5)
	cmovw t1,s6,s0
	sw t1,(s5)
# 0 "" 2
 #NO_APP
	lw	a3,12(sp)
	mv	a2,s0
	li	a1,1
	addi	a0,s4,%lo(.LC0)
	addiw	s0,s0,1
	call	printf
	addi	s1,s1,4
	bne	s0,s2,.L3
.L7:
	ld	ra,72(sp)
	ld	s0,64(sp)
	lw	a0,12(sp)
	ld	s1,56(sp)
	ld	s2,48(sp)
	ld	s3,40(sp)
	ld	s4,32(sp)
	ld	s5,24(sp)
	ld	s6,16(sp)
	addi	sp,sp,80
	jr	ra
	.size	BinarySearch, .-BinarySearch
	.section	.text.startup,"ax",@progbits
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-64
	li	a0,4096
	sd	s1,40(sp)
	sd	ra,56(sp)
	sd	s0,48(sp)
	sd	s2,32(sp)
	sd	s3,24(sp)
	call	malloc
	mv	s1,a0
	mv	a4,a0
	li	a5,0
	li	a3,1024
.L9:
	sw	a5,0(a4)
	addiw	a5,a5,1
	addi	a4,a4,4
	bne	a5,a3,.L9
	li	a0,0
	call	srand
	call	rand
	sraiw	s0,a0,31
	srliw	a5,s0,21
	addw	s0,a5,a0
	andi	s0,s0,2047
	subw	s0,s0,a5
 #APP
# 37 "binary_search.c" 1
	csrrs s2,cycle,x0
# 0 "" 2
 #NO_APP
	mv	a1,s0
	mv	a0,s1
	call	BinarySearch
 #APP
# 39 "binary_search.c" 1
	csrrs s3,cycle,x0
# 0 "" 2
 #NO_APP
	li	a5,-1
	beq	a0,a5,.L14
	mv	a2,a0
	lui	a0,%hi(.LC2)
	li	a3,1023
	mv	a1,s0
	addi	a0,a0,%lo(.LC2)
	call	printf
.L11:
	lui	a0,%hi(.LC3)
	subw	a1,s3,s2
	addi	a0,a0,%lo(.LC3)
	call	printf
	li	a5,3
	sw	a5,12(sp)
	li	a4,2
	li	a5,1
	addi	a3,sp,12
 #APP
# 48 "binary_search.c" 1
	lw t1,(a3)
	cmovw t1,a5,a4
	sw t1,(a3)
# 0 "" 2
 #NO_APP
	lw	a3,12(sp)
	lui	a0,%hi(.LC0)
	li	a2,2
	li	a1,1
	addi	a0,a0,%lo(.LC0)
	call	printf
	mv	a0,s1
	call	free
	ld	ra,56(sp)
	ld	s0,48(sp)
	ld	s1,40(sp)
	ld	s2,32(sp)
	ld	s3,24(sp)
	li	a0,0
	addi	sp,sp,64
	jr	ra
.L14:
	lui	a0,%hi(.LC1)
	li	a2,1023
	mv	a1,s0
	addi	a0,a0,%lo(.LC1)
	call	printf
	j	.L11
	.size	main, .-main
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align	3
.LC0:
	.string	"%d %d %d\n"
	.zero	6
.LC1:
	.string	"Value %d is not in the array [0:%d].\n"
	.zero	2
.LC2:
	.string	"Value %d is at index %d in the array [0:%d].\n"
	.zero	2
.LC3:
	.string	"%d\n"
	.ident	"GCC: (GNU) 7.2.0"
