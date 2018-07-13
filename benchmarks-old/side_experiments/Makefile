all: scan path_oram oram_stash_in_1L2 PLS_1L1 PLS_2L1 PLS_4L1 PLS_1L2 PLS_2L2

scan:
	gcc ../../primitives/lib/asm.c ../../primitives/scan_oram/scan_oram.c scan.c -static -Wall -O3 -m32 -D M2S -o scan.o

path_oram:
	gcc ../../primitives/lib/asm.c ../../primitives/lib/misc.c ../../primitives/path_oram/obliv_path_oram.c ../../primitives/sort/merge_sort.c ../../primitives/sort/bitonic_sort.c ../../primitives/scan_oram/scan_oram.c path_oram.c -static -O3 -Wall -m32  -D M2S -o path_oram.o

path_oram_local:
	gcc ../../primitives/lib/asm.c ../../primitives/lib/misc.c ../../primitives/path_oram/obliv_path_oram.c ../../primitives/sort/merge_sort.c ../../primitives/sort/bitonic_sort.c ../../primitives/scan_oram/scan_oram.c path_oram.c -static -O3 -Wall -D M2S -o path_oram.out

oram_stash_in_1L2:
	gcc ../../primitives/lib/asm.c ../../primitives/lib/misc.c ../../primitives/cisc_insts/cisc.c ../../primitives/lib/freelist.c ../../primitives/partpls_path_oram/stash_path_oram.c oram_stash_in_1L2.c -static -O3 -Wall  -m32 -D M2S -o oram_stash_in_1L2.o

PLS_1L1:
	gcc ../../primitives/lib/asm.c ../../primitives/lib/freelist.c ../../primitives/cisc_insts/cisc.c PLS_1L1.c -static -Wall -O3 -m32 -D M2S -o PLS_1L1.o

PLS_2L1:
	gcc ../../primitives/lib/asm.c ../../primitives/lib/freelist.c ../../primitives/cisc_insts/cisc.c PLS_2L1.c -static -Wall -O3 -m32 -D M2S -o PLS_2L1.o

PLS_4L1:
	gcc ../../primitives/lib/asm.c ../../primitives/lib/freelist.c ../../primitives/cisc_insts/cisc.c PLS_4L1.c -static -Wall -O3 -m32 -D M2S -o PLS_4L1.o

PLS_1L2:
	gcc ../../primitives/lib/asm.c ../../primitives/lib/freelist.c ../../primitives/cisc_insts/cisc.c PLS_1L2.c -static -Wall -O3 -m32 -D M2S -o PLS_1L2.o

PLS_2L2:
	gcc ../../primitives/lib/asm.c ../../primitives/lib/freelist.c ../../primitives/cisc_insts/cisc.c PLS_2L2.c -static -Wall -O3 -m32 -D M2S -o PLS_2L2.o

clean:
	rm *.o
