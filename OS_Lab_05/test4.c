#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(1, "Usage: test_seq <algo_number>\n");
        exit();
    }

    int algo = atoi(argv[1]);
    char *pages = sbrk(6 * PGSIZE);

    if(pages == (char*)-1) {
        exit();
    }

    for(int i = 0; i < 5; i++) {
        paging_write(pages + (i * PGSIZE), 0, algo);
    }

    printf(1, "--- Program 4: Specific Sequence Access (Algo: %d) ---\n", algo);
    reset_paging_metrics();
    uint start_ticks = uptime();

    for(int i = 0; i < 100; i++) {
        paging_read(pages + (0 * PGSIZE), algo);
        paging_read(pages + (1 * PGSIZE), algo);
        paging_read(pages + (2 * PGSIZE), algo);
        paging_read(pages + (3 * PGSIZE), algo);
        paging_read(pages + (0 * PGSIZE), algo);
        paging_read(pages + (1 * PGSIZE), algo);
        paging_read(pages + (4 * PGSIZE), algo);
    }

    uint end_ticks = uptime();
    
    paging_metrics();
    printf(1, "Execution Time: %d ticks\n", end_ticks - start_ticks);
    
    exit();
}