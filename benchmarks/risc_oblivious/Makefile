x86: bsearch findmax intsort kmeans matmult pagerank oram dnn pq dijkstra
#rv64: bsearch_rv64 findmax_rv64 intsort_rv64 kmeans_rv64 matmult_rv64 pagerank_rv64 oram_rv64 dnn_rv64 pq_rv64 dijkstra_rv64
rv64: bsearch_rv64 findmax_rv64 intsort_rv64 kmeans_rv64 matmult_rv64 pagerank_rv64 dnn_rv64 pq_rv64 dijkstra_rv64


GCC=gcc
RV64_GCC=riscv64-unknown-elf-gcc

bsearch:
	$(GCC) ../include/asm.c ../include/misc.c binary_search.c -static -O3 -Wall -o binary_search.o

findmax:
	$(GCC) ../include/asm.c find_max.c -static -O3 -Wall -o find_max.o

intsort:
	$(GCC) ../include/asm.c ../../../primitives/sort/bitonic_sort.c int_sort.c -O3 -Wall -o int_sort.o

kmeans:
	$(GCC) ../../../primitives/scan_oram/scan_oram.c ../include/asm.c kmeans.c -O3 -Wall -o kmeans.o -lm

matmult:
	$(GCC) matrix_mult.c -O3 -Wall -o matrix_mult.o

pagerank:
	$(GCC) ../../../primitives/sort/bitonic_sort.c ../../../primitives/sort/merge_sort.c ../include/asm.c page_rank.c -O3 -Wall -o page_rank.o  -g 

oram:
	$(GCC) ../../../primitives/include/asm.c ../../../primitives/include/misc.c ../../../primitives/path_oram/obliv_path_oram.c ../../../primitives/sort/merge_sort.c ../../../primitives/sort/bitonic_sort.c ../../../primitives/scan_oram/scan_oram.c oram.c -static -O3 -Wall -o oram.o

dnn:
	$(GCC) dnn.c -static -O3 -Wall -o dnn.o -lm

pq:
	$(GCC) ../../../primitives/include/asm.c ../../../primitives/include/misc.c ../../../primitives/scan_oram/scan_oram.c PQ.c -static -O3 -Wall -o PQ.o

dijkstra:
	$(GCC) ../../../primitives/include/asm.c ../../../primitives/include/misc.c ../../../primitives/scan_oram/scan_oram.c ../../../primitives/sort/merge_sort.c dijkstra.c -static -O3 -Wall -o dijkstra.o

# For RISCV(boom/spike)
bsearch_rv64:
	$(RV64_GCC) ../include/asm.c binary_search.c -O3 -Wall -o binary_search.o

findmax_rv64:
	$(RV64_GCC) ../include/asm.c find_max.c -O3 -Wall -o find_max.o

intsort_rv64:
	$(RV64_GCC) ../include/asm.c int_sort.c -O3 -Wall -o int_sort.o

kmeans_rv64:
	$(RV64_GCC) ../include/scan_oram/scan_oram.c ../include/asm.c kmeans.c -O3 -Wall -o kmeans.o -lm

matmult_rv64:
	$(RV64_GCC) matrix_mult.c -O3 -Wall -o matrix_mult.o

pagerank_rv64:
	$(RV64_GCC) ../include/asm.c page_rank.c -O3 -Wall -o page_rank.o

oram_rv64:
	$(RV64_GCC) ../include/asm.c ../../../primitives/path_oram/obliv_path_oram.c ../../../primitives/sort/merge_sort.c ../../../primitives/sort/bitonic_sort.c ../../../primitives/scan_oram/scan_oram.c oram.c -static -O3 -Wall -D M2S -m32 -o oram.o

dnn_rv64:
	$(RV64_GCC) dnn.c -static -O3 -Wall -o dnn.o -lm

pq_rv64:
	$(RV64_GCC) ../include/asm.c ../include/scan_oram/scan_oram.c PQ.c -static -O3 -Wall -o PQ.o

dijkstra_rv64:
	$(RV64_GCC) ../include/asm.c ../include/scan_oram/scan_oram.c dijkstra.c -static -O3 -Wall -o dijkstra.o

clean:
	rm *.o
