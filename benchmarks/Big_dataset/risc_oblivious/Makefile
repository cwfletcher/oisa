all: bsearch findmax intsort kmeans matmult pagerank oram dnn pq dijkstra

sim: bsearch_sim findmax_sim intsort_sim kmeans_sim matmult_sim pagerank_sim oram_sim dnn_sim pq_sim dijkstra_sim

# For local run
bsearch:
	gcc ../../../primitives/lib/asm.c ../../../primitives/lib/misc.c ../../../primitives/path_oram/obliv_path_oram.c ../../../primitives/sort/merge_sort.c ../../../primitives/sort/bitonic_sort.c ../../../primitives/scan_oram/scan_oram.c binary_search.c -static -O3 -Wall -o binary_search.o

findmax:
	gcc ../../../primitives/lib/asm.c find_max.c -static -O3 -Wall -o find_max.o

intsort:
	gcc ../../../primitives/lib/asm.c ../../../primitives/sort/bitonic_sort.c int_sort.c -O3 -Wall -o int_sort.o

kmeans:
	gcc ../../../primitives/scan_oram/scan_oram.c ../../../primitives/lib/asm.c kmeans.c -O3 -Wall -o kmeans.o -lm

matmult:
	gcc matrix_mult.c -O3 -Wall -o matrix_mult.o

pagerank:
	gcc ../../../primitives/sort/bitonic_sort.c ../../../primitives/sort/merge_sort.c ../../../primitives/lib/asm.c page_rank.c -O3 -Wall -o page_rank.o

oram:
	gcc ../../../primitives/lib/asm.c ../../../primitives/lib/misc.c ../../../primitives/path_oram/obliv_path_oram.c ../../../primitives/sort/merge_sort.c ../../../primitives/sort/bitonic_sort.c ../../../primitives/scan_oram/scan_oram.c oram.c -static -O3 -Wall -o oram.o

dnn:
	gcc dnn.c -static -O3 -Wall -o dnn.o -lm

pq:
	gcc ../../../primitives/lib/asm.c ../../../primitives/lib/misc.c ../../../primitives/scan_oram/scan_oram.c PQ.c -static -O3 -Wall -o PQ.o

dijkstra:
	gcc ../../../primitives/lib/asm.c ../../../primitives/lib/misc.c ../../../primitives/scan_oram/scan_oram.c ../../../primitives/sort/merge_sort.c dijkstra.c -static -O3 -Wall -o dijkstra.o

# For M2S
bsearch_sim:
	gcc ../../../primitives/lib/asm.c ../../../primitives/lib/misc.c ../../../primitives/path_oram/obliv_path_oram.c ../../../primitives/sort/merge_sort.c ../../../primitives/sort/bitonic_sort.c ../../../primitives/scan_oram/scan_oram.c binary_search.c -static -O3 -Wall -D M2S -m32 -o binary_search.o

findmax_sim:
	gcc ../../../primitives/lib/asm.c find_max.c -static -O3 -Wall -D M2S -m32 -o find_max.o

intsort_sim:
	gcc ../../../primitives/sort/bitonic_sort.c int_sort.c -O3 -Wall -D M2S -m32 -o int_sort.o

kmeans_sim:
	gcc ../../../primitives/scan_oram/scan_oram.c ../../../primitives/lib/asm.c kmeans.c -O3 -Wall -D M2S -m32 -o kmeans.o -lm

matmult_sim:
	gcc matrix_mult.c -O3 -Wall -D M2S -m32 -o matrix_mult.o

pagerank_sim:
	gcc ../../../primitives/sort/bitonic_sort.c ../../../primitives/sort/merge_sort.c ../../../primitives/lib/asm.c page_rank.c -O3 -Wall -D M2S -m32 -o page_rank.o

oram_sim:
	gcc ../../../primitives/lib/asm.c ../../../primitives/lib/misc.c ../../../primitives/path_oram/obliv_path_oram.c ../../../primitives/sort/merge_sort.c ../../../primitives/sort/bitonic_sort.c ../../../primitives/scan_oram/scan_oram.c oram.c -static -O3 -Wall -D M2S -m32 -o oram.o

dnn_sim:
	gcc dnn.c -static -O3 -Wall -D M2S -m32 -o dnn.o -lm

pq_sim:
	gcc ../../../primitives/lib/asm.c ../../../primitives/lib/misc.c ../../../primitives/scan_oram/scan_oram.c PQ.c -static -O3 -Wall -D M2S -m32 -o PQ.o

dijkstra_sim:
	gcc ../../../primitives/lib/asm.c ../../../primitives/lib/misc.c ../../../primitives/scan_oram/scan_oram.c ../../../primitives/sort/merge_sort.c dijkstra.c -static -O3 -Wall -D M2S -m32 -o dijkstra.o

clean:
	rm *.o
