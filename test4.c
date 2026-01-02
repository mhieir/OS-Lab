#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096
int main() {
    char *pages = sbrk(6 * PGSIZE);

    printf(1, "--- Program 4: Specific Sequence Access ---\n");
    for(int i = 0; i < 100; i++) {
        paging_read(pages + (0 * PGSIZE));
        paging_read(pages + (1 * PGSIZE));
        paging_read(pages + (2 * PGSIZE));
        paging_read(pages + (3 * PGSIZE));
        paging_read(pages + (0 * PGSIZE));
        paging_read(pages + (1 * PGSIZE));
        paging_read(pages + (4 * PGSIZE));
    }
    
    paging_metrics();
    exit();
}