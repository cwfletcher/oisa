x86: bsearch findmax intsort kmeans matmult pagerank oram dnn pq dijkstra
riscv64: bsearch_riscv findmax_riscv intsort_riscv kmeans_riscv matmult_riscv pagerank_riscv oram_riscv dnn_riscv pq_riscv dijkstra_riscv

boom:  bsearch_boom findmax_boom intsort_boom kmeans_boom matmult_boom pagerank_boom oram_boom dnn_boom pq_boom dijkstra_boom
spike: bsearch_sim findmax_sim intsort_sim kmeans_sim matmult_sim pagerank_sim oram_sim dnn_sim pq_sim dijkstra_sim

GCC=gcc
RISCV64_GCC=riscv64-unknown-elf-gcc

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
bsearch_riscv:
	$(RISCV64_GCC) -static -O3 -Wall binary_search.c -o binary_search.o

findmax_riscv:
	$(RISCV64_GCC) -static -O3 -Wall find_max.c -o find_max.o

intsort_riscv:
	$(RISCV64_GCC) -static -O3 -Wall ../lib/asm.c ../../../primitives/sort/merge_sort.c int_sort.c -o int_sort.o

kmeans_riscv:
	$(RISCV64_GCC) -static -O3 -Wall kmeans.c -o kmeans.o -lm

matmult_riscv:
	$(RISCV64_GCC) -static -O3 -Wall matrix_mult.c -o matrix_mult.o

pagerank_riscv:
	$(RISCV64_GCC) -static -O3 -Wall page_rank.c -o page_rank.o

oram_riscv:
	$(RISCV64_GCC) -static -O3 -Wall ../lib/asm.c ../lib/misc.c ../../../primitives/path_oram/nobliv_path_oram.c oram.c -o oram.o

dnn_riscv:
	$(RISCV64_GCC) -static -O3 -Wall dnn.c -o dnn.o -lm

pq_riscv:
	$(RISCV64_GCC) -static -O3 -Wall ../lib/asm.c PQ.c -o PQ.o

dijkstra_riscv:
	$(RISCV64_GCC) -static -O3 -Wall dijkstra.c -o dijkstra.o

clean:
	rm *.o
