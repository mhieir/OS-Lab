#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(1, "Usage: cyclic_test <algo_number>\n");
        printf(1, "0: FIFO, 1: LRU, 2: LFU, 3: Clock\n");
        exit();
    }

    int algo = atoi(argv[1]);
    char *p = sbrk(6 * PGSIZE);
    if(p == (char*)-1){
        printf(1, "sbrk failed\n");
        exit();
    }

    printf(1, "--- Program 2: Cyclic 5-Page Access using Algo %d ---\n", algo);
    for(int i = 0; i < 5; i++) {
        paging_write(p + (i * PGSIZE), i * 100, algo);
    }
    
    uint start_ticks = uptime();
    reset_paging_metrics();
    for(int loop = 0; loop < 4; loop++) {
        for(int i = 0; i < 5; i++) {
            int val = paging_read(p + (i * PGSIZE), algo); 
            printf(1, "Loop %d: Read Page %d (val: %d)\n", loop, i, val);
        }
    }
    
    uint end_ticks = uptime();
    
    paging_metrics();
    printf(1, "Execution Time: %d ticks\n", end_ticks - start_ticks);
    exit();
}