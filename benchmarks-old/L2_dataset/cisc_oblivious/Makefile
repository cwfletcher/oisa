all: bsearch findmax intsort kmeans matmult pagerank oram dnn pq dijkstra

sim: bsearch_sim findmax_sim intsort_sim kmeans_sim matmult_sim pagerank_sim oram_sim dnn_sim pq_sim dijkstra_sim

# For M2S
bsearch_sim:
	gcc -static -O3 -Wall -D M2S -m32 binary_search/binary_search.c ../../../primitives/lib/asm.c ../../../primitives/lib/misc.c ../../../primitives/cisc_insts/cisc.c ../../../primitives/lib/freelist.c -o binary_search.o

findmax_sim:
	gcc -static -O3 -Wall -D M2S -m32 find_max/find_max.c ../../../primitives/lib/asm.c -o find_max.o

intsort_sim:
	gcc -static -O3 -Wall -D M2S -m32 int_sort/int_sort.c ../../../primitives/lib/asm.c ../../../primitives/cisc_insts/cisc.c ../../../primitives/lib/freelist.c ../../../primitives/sort/merge_sort_pls.c -o int_sort.o

kmeans_sim:
	gcc -static -O3 -Wall -D M2S -m32 kmeans/kmeans.c ../../../primitives/lib/asm.c ../../../primitives/cisc_insts/cisc.c ../../../primitives/lib/freelist.c -o kmeans.o -lm

matmult_sim:
	gcc -static -O3 -Wall -D M2S -m32 matrix_mult/matrix_mult.c -o matrix_mult.o

pagerank_sim:
	gcc -static -O3 -Wall -D M2S -m32 page_rank/page_rank.c ../../../primitives/lib/asm.c ../../../primitives/cisc_insts/cisc.c ../../../primitives/lib/freelist.c -o page_rank.o

oram_sim:
	gcc -static -O3 -Wall -D M2S -m32 oram/oram.c ../../../primitives/lib/asm.c ../../../primitives/cisc_insts/cisc.c ../../../primitives/lib/freelist.c ../../../primitives/lib/misc.c ../../../primitives/pls_path_oram/stash_posmap_path_oram.c -o oram.o

dnn_sim:
	gcc -static -O3 -Wall -D M2S -m32 dnn/dnn.c -o dnn.o -lm

pq_sim:
	gcc -static -O3 -Wall -D M2S -m32 PQ/PQ.c ../../../primitives/lib/asm.c ../../../primitives/lib/misc.c ../../../primitives/cisc_insts/cisc.c ../../../primitives/lib/freelist.c -o PQ.o

dijkstra_sim:
	gcc -static -O3 -Wall -D M2S -m32 dijkstra/dijkstra.c ../../../primitives/lib/asm.c ../../../primitives/lib/misc.c ../../../primitives/cisc_insts/cisc.c ../../../primitives/lib/freelist.c ../../../primitives/sort/merge_sort.c -o dijkstra.o

clean:
	rm *.o
