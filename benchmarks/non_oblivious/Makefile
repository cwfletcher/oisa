x86: bsearch findmax intsort kmeans matmult pagerank oram dnn pq dijkstra
#rv64: bsearch_rv64 findmax_rv64 intsort_rv64 kmeans_rv64 matmult_rv64 pagerank_rv64 oram_rv64 dnn_rv64 pq_rv64 dijkstra_rv64
rv64: bsearch_rv64 findmax_rv64 intsort_rv64 kmeans_rv64 matmult_rv64 pagerank_rv64 dnn_rv64 pq_rv64 dijkstra_rv64

#boom:  bsearch_boom findmax_boom intsort_boom kmeans_boom matmult_boom pagerank_boom oram_boom dnn_boom pq_boom dijkstra_boom
#spike: bsearch_rv64 findmax_rv64 intsort_rv64 kmeans_rv64 matmult_rv64 pagerank_rv64 oram_rv64 dnn_rv64 pq_rv64 dijkstra_rv64

GCC=gcc
RV64_GCC=riscv64-unknown-elf-gcc

# For local run
bsearch:
	$(GCC) -static -O3 -Wall binary_search.c -o binary_search.o

findmax:
	$(GCC) -static -O3 -Wall find_max.c -o find_max.o

intsort:
	$(GCC) -static -O3 -Wall ../lib/asm.c ../../../primitives/sort/merge_sort.c int_sort.c -o int_sort.o

kmeans:
	$(GCC) -static -O3 -Wall kmeans.c -o kmeans.o -lm

matmult:
	$(GCC) -static -O3 -Wall matrix_mult.c -o matrix_mult.o

pagerank:
	$(GCC) -static -O3 -Wall page_rank.c -o page_rank.o

oram:
	$(GCC) -static -O3 -Wall ../lib/asm.c ../misc.c ../../../primitives/path_oram/nobliv_path_oram.c oram.c -o oram.o

dnn:
	$(GCC) -static -O3 -Wall dnn.c -o dnn.o -lm

pq:
	$(GCC) -static -O3 -Wall PQ.c -o PQ.o

dijkstra:
	$(GCC) -static -O3 -Wall dijkstra.c -o dijkstra.o

# For RISCV(boom/spike)
bsearch_rv64:
	$(RV64_GCC)  -O3 -Wall binary_search.c -o binary_search.o

findmax_rv64:
	$(RV64_GCC)  -O3 -Wall find_max.c -o find_max.o

intsort_rv64:
	$(RV64_GCC)  -O3 -Wall ../include/asm.c int_sort.c -o int_sort.o

kmeans_rv64:
	$(RV64_GCC)  -O3 -Wall kmeans.c -o kmeans.o -lm

matmult_rv64:
	$(RV64_GCC)  -O3 -Wall matrix_mult.c -o matrix_mult.o

pagerank_rv64:
	$(RV64_GCC)  -O3 -Wall page_rank.c -o page_rank.o

oram_rv64:
	$(RV64_GCC)  -O3 -Wall ../include/asm.c ../../../primitives/path_oram/nobliv_path_oram.c oram.c -o oram.o

dnn_rv64:
	$(RV64_GCC)  -O3 -Wall dnn.c -o dnn.o -lm

pq_rv64:
	$(RV64_GCC)  -O3 -Wall ../include/asm.c PQ.c -o PQ.o

dijkstra_rv64:
	$(RV64_GCC)  -O3 -Wall dijkstra.c -o dijkstra.o

clean:
	rm *.o
