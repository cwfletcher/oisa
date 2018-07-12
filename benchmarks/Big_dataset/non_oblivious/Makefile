all: bsearch findmax intsort kmeans matmult pagerank oram dnn pq dijkstra

sim: bsearch_sim findmax_sim intsort_sim kmeans_sim matmult_sim pagerank_sim oram_sim dnn_sim pq_sim dijkstra_sim

# For local run
bsearch:
	gcc -static -O3 -Wall binary_search.c -o binary_search.o

findmax:
	gcc -static -O3 -Wall find_max.c -o find_max.o

intsort:
	gcc -static -O3 -Wall ../../../primitives/lib/asm.c ../../../primitives/sort/merge_sort.c int_sort.c -o int_sort.o

kmeans:
	gcc -static -O3 -Wall kmeans.c -o kmeans.o -lm

matmult:
	gcc -static -O3 -Wall matrix_mult.c -o matrix_mult.o

pagerank:
	gcc -static -O3 -Wall page_rank.c -o page_rank.o

oram:
	gcc -static -O3 -Wall ../../../primitives/lib/asm.c ../../../primitives/lib/misc.c ../../../primitives/path_oram/nobliv_path_oram.c oram.c -o oram.o

dnn:
	gcc -static -O3 -Wall dnn.c -o dnn.o -lm

pq:
	gcc -static -O3 -Wall PQ.c -o PQ.o

dijkstra:
	gcc -static -O3 -Wall dijkstra.c -o dijkstra.o

# For M2S
bsearch_sim:
	gcc -static -O3 -Wall -D M2S -m32 binary_search.c -o binary_search.o

findmax_sim:
	gcc -static -O3 -Wall -D M2S -m32 find_max.c -o find_max.o

intsort_sim:
	gcc -static -O3 -Wall -D M2S -m32 ../../../primitives/sort/merge_sort.c int_sort.c -o int_sort.o

kmeans_sim:
	gcc -static -O3 -Wall -D M2S -m32 ../../../primitives/lib/asm.c kmeans.c -o kmeans.o -lm

matmult_sim:
	gcc -static -O3 -Wall -D M2S -m32 ../../../primitives/lib/asm.c matrix_mult.c -o matrix_mult.o

pagerank_sim:
	gcc -static -O3 -Wall -D M2S -m32 page_rank.c -o page_rank.o

oram_sim:
	gcc -static -O3 -Wall -D M2S -m32 ../../../primitives/lib/asm.c ../../../primitives/lib/misc.c ../../../primitives/path_oram/nobliv_path_oram.c oram.c -o oram.o

dnn_sim:
	gcc -static -O3 -Wall -D M2S -m32 ../../../primitives/lib/asm.c dnn.c -o dnn.o -lm

pq_sim:
	gcc -static -O3 -Wall -D M2S -m32 ../../../primitives/lib/asm.c PQ.c -o PQ.o

dijkstra_sim:
	gcc -static -O3 -Wall -D M2S -m32 dijkstra.c -o dijkstra.o

clean:
	rm *.o
