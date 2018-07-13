	.file	"binary_search.c"
	.option nopic
	.section	.sdata,"aw",@progbits
	.align	2
	.type	N, @object
	.size	N, 4
N:
	.word	1024
	.align	2
	.type	seed, @object
	.size	seed, 4
seed:
	.word	2
	.local	zero
	.comm	zero,4,4
	.section	.rodata
	.align	3
.LC0:
	.string	"%d, %d, %d\n"
	.text
	.align	2
	.globl	BinarySearch
	.type	BinarySearch, @function
BinarySearch:
	addi	sp,sp,-48
	sd	ra,40(sp)
	sd	s0,32(sp)
	addi	s0,sp,48
	sd	a0,-40(s0)
	mv	a5,a1
	sw	a5,-44(s0)
	li	a5,-1
	sw	a5,-28(s0)
	sw	zero,-20(s0)
	j	.L2
.L4:
	lw	a5,-20(s0)#i
	slli	a5,a5,2
	ld	a4,-40(s0)
	add	a5,a4,a5
	lw	a4,0(a5)
	lw	a5,-44(s0)
	sext.w	a5,a5
	sub	a5,a5,a4
# a5 == arr[i], a4 == x
	seqz	a5,a5
# a5 == match
	andi	a5,a5,0xff
	sw	a5,-24(s0)
	lw	a5,-24(s0)
	sext.w	a5,a5
	beqz	a5,.L3
	lw	a3,-28(s0)# answer
	lw	a4,-20(s0)# i
	lw	a5,-24(s0)# match
	mv	a2,a4#i
	mv	a1,a5#match
	lui	a5,%hi(.LC0)
	addi	a0,a5,%lo(.LC0)
	call	printf
	lw	a5,-24(s0)#match
	lw	a4,-20(s0)#i
	addi	a3,s0,-28
 #APP
# 23 "binary_search.c" 1
	ld t1,(a3)
	cmov t1,a5,a4
	sd t1,(a3)
# 0 "" 2
 #NO_APP
	lw	a3,-28(s0)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	mv	a2,a4
	mv	a1,a5
	lui	a5,%hi(.LC0)
	addi	a0,a5,%lo(.LC0)
	call	printf
.L3:
	lw	a5,-20(s0)
	addiw	a5,a5,1
	sw	a5,-20(s0)
.L2:
	lui	a5,%hi(N)
	lw	a4,%lo(N)(a5)
	lw	a5,-20(s0)
	sext.w	a5,a5
	blt	a5,a4,.L4
	lw	a5,-28(s0)
	mv	a0,a5
	ld	ra,40(sp)
	ld	s0,32(sp)
	addi	sp,sp,48
	jr	ra
	.size	BinarySearch, .-BinarySearch
	.section	.rodata
	.align	3
.LC1:
	.string	"%d %d %d\n"
	.align	3
.LC2:
	.string	"Value %d is not in the array [0:%d]\n"
	.align	3
.LC3:
	.string	"Value %d is at index %d in the array [0:%d\n]"
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-64
	sd	ra,56(sp)
	sd	s0,48(sp)
	addi	s0,sp,64
	li	a5,1
	sw	a5,-24(s0)
	lui	a5,%hi(N)
	lw	a5,%lo(N)(a5)
	mv	a4,a5
	lw	a5,-24(s0)
	mul	a5,a4,a5
	slli	a5,a5,2
	mv	a0,a5
	call	malloc
	mv	a5,a0
	sd	a5,-32(s0)
	sw	zero,-20(s0)
	j	.L7
.L8:
	lw	a5,-20(s0)
	slli	a5,a5,2
	ld	a4,-32(s0)
	add	a5,a4,a5
	lw	a4,-20(s0)
	sw	a4,0(a5)
	lw	a5,-20(s0)
	addiw	a5,a5,1
	sw	a5,-20(s0)
.L7:
	lui	a5,%hi(N)
	lw	a5,%lo(N)(a5)
	lw	a4,-24(s0)
	mulw	a5,a4,a5
	sext.w	a4,a5
	lw	a5,-20(s0)
	sext.w	a5,a5
	blt	a5,a4,.L8
	lui	a5,%hi(seed)
	lw	a5,%lo(seed)(a5)
	sext.w	a5,a5
	mv	a0,a5
	call	srand
	call	rand
	mv	a5,a0
	mv	a4,a5
	lui	a5,%hi(N)
	lw	a5,%lo(N)(a5)
	slliw	a5,a5,1
	sext.w	a5,a5
	remw	a5,a4,a5
	sw	a5,-36(s0)
	lw	a5,-36(s0)
	mv	a1,a5
	ld	a0,-32(s0)
	call	BinarySearch
	mv	a5,a0
	sw	a5,-40(s0)
	sw	zero,-44(s0)
	li	a5,12
	sw	a5,-48(s0)
	li	a5,14
	sw	a5,-52(s0)
	lw	a4,-44(s0)
	lw	a3,-48(s0)
	lw	a5,-52(s0)
 #APP
# 43 "binary_search.c" 1
	cmov a5, a4, a3
# 0 "" 2
 #NO_APP
	sw	a5,-52(s0)
	lw	a3,-52(s0)
	lw	a4,-48(s0)
	lw	a5,-44(s0)
	mv	a2,a4
	mv	a1,a5
	lui	a5,%hi(.LC1)
	addi	a0,a5,%lo(.LC1)
	call	printf
	lw	a5,-40(s0)
	sext.w	a4,a5
	li	a5,-1
	bne	a4,a5,.L9
	lui	a5,%hi(N)
	lw	a5,%lo(N)(a5)
	addiw	a5,a5,-1
	sext.w	a4,a5
	lw	a5,-36(s0)
	mv	a2,a4
	mv	a1,a5
	lui	a5,%hi(.LC2)
	addi	a0,a5,%lo(.LC2)
	call	printf
	j	.L10
.L9:
	lui	a5,%hi(N)
	lw	a5,%lo(N)(a5)
	addiw	a5,a5,-1
	sext.w	a4,a5
	lw	a5,-36(s0)
	mv	a2,a4
	mv	a1,a5
	lui	a5,%hi(.LC3)
	addi	a0,a5,%lo(.LC3)
	call	printf
.L10:
	ld	a0,-32(s0)
	call	free
	li	a5,0
	mv	a0,a5
	ld	ra,56(sp)
	ld	s0,48(sp)
	addi	sp,sp,64
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 7.2.0"
