#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf(1, "Usage: test_paging <algo_number>\n");
        printf(1, "0: FIFO, 1: LRU, 2: LFU, 3: Clock\n");
        exit();
    }

    int algo = atoi(argv[1]);

    char *p = sbrk(12 * PGSIZE);
    if(p == (char*)-1){
        printf(1, "sbrk failed\n");
        exit();
    }

    printf(1, "--- Program 1: Testing Algorithm %d ---\n", algo);
    uint start_ticks = uptime();
    for(int i = 0; i < 11; i++) {
        char *addr = p + (i * PGSIZE);
        paging_write(addr, i * 10, algo);
        printf(1, "Accessed Page %d (Virtual Addr: %p)\n", i, addr);
    }
    
    uint end_ticks = uptime();
    
    paging_metrics();
    printf(1, "Execution Time: %d ticks\n", end_ticks - start_ticks);
    exit();
}