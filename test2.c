#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096
int main() {
    char *p = sbrk(6 * PGSIZE);
    
    printf(1, "--- Program 2: Cyclic 5-Page Access ---\n");
    for(int loop = 0; loop < 4; loop++) {
        for(int i = 0; i < 5; i++) {
            int val = paging_read(p + (i * PGSIZE)); // Read cyclicly [cite: 25, 55]
            printf(1, "Loop %d: Read Page %d (val: %d)\n", loop, i, val);
        }
    }
    
    paging_metrics();
    exit();
}