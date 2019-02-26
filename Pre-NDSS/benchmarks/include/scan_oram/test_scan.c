#include <stdio.h>
#include <stdlib.h>
#include "scan_oram.h"

typedef struct {
    int fa;
    int fb;
    int fc;
} Ent;

int main(){
    int N = 10;
    Ent* entries = (Ent*) malloc(sizeof(Ent) * N);
    for(int i = 0; i < N; i++){
        entries[i].fa = i;
        entries[i].fb = i;
        entries[i].fc = i;
    }
    Ent entry_empty;
    Ent entry_full;
    entry_full.fa = 111;
    entry_full.fb = 222;
    entry_full.fc = 333;
    printf("\n@@@ Array is:\n");
    for(int i = 0; i < N; i++)
        printf("entry[%d].fa = %d; entry[%d].fb = %d; entry[%d].fc = %d\n",
                i, entries[i].fa, i, entries[i].fb, i, entries[i].fc);
    printf("\n### entry_full is:\n");
    printf("entry.fa = %d; entry.fb = %d; entry.fc = %d\n",
                entry_full.fa, entry_full.fb, entry_full.fc);
    printf("\n### entry_empty is:\n");
    printf("entry.fa = %d; entry.fb = %d; entry.fc = %d\n",
                entry_empty.fa, entry_empty.fb, entry_empty.fc);

    ScanORAM_Read((int*)entries, N, sizeof(Ent)/sizeof(int), (int*)&entry_empty, 5);
    printf("\nRead done\n");
    printf("\n@@@ Array is:\n");
    for(int i = 0; i < N; i++)
        printf("entry[%d].fa = %d; entry[%d].fb = %d; entry[%d].fc = %d\n",
                i, entries[i].fa, i, entries[i].fb, i, entries[i].fc);
    printf("\n### entry_empty is:\n");
    printf("entry.fa = %d; entry.fb = %d; entry.fc = %d\n",
                entry_empty.fa, entry_empty.fb, entry_empty.fc);
    
    ScanORAM_Write((int*)entries, N, sizeof(Ent)/sizeof(int), (int*)&entry_full, 5);
    printf("\nWrite done\n");
    printf("\n@@@ Array is:\n");
    for(int i = 0; i < N; i++)
        printf("entry[%d].fa = %d; entry[%d].fb = %d; entry[%d].fc = %d\n",
                i, entries[i].fa, i, entries[i].fb, i, entries[i].fc);
    printf("\n### entry_empty is:\n");
    printf("entry.fa = %d; entry.fb = %d; entry.fc = %d\n",
                entry_empty.fa, entry_empty.fb, entry_empty.fc);

    ScanORAM_Read((int*)entries, N, sizeof(Ent)/sizeof(int), (int*)&entry_empty, 5);
    printf("\nRead done\n");
    printf("\n@@@ Array is:\n");
    for(int i = 0; i < N; i++)
        printf("entry[%d].fa = %d; entry[%d].fb = %d; entry[%d].fc = %d\n",
                i, entries[i].fa, i, entries[i].fb, i, entries[i].fc);
    printf("\n### entry_empty is:\n");
    printf("entry.fa = %d; entry.fb = %d; entry.fc = %d\n",
                entry_empty.fa, entry_empty.fb, entry_empty.fc);

    return 0;
}
