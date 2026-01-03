#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(1, "Usage: test_hotcold <algo_number>\n");
        exit();
    }

    int algo = atoi(argv[1]);
    char *hot = sbrk(3 * PGSIZE);
    char *cold = sbrk(5 * PGSIZE);

    if(hot == (char*)-1 || cold == (char*)-1){
        exit();
    }

    for(int i = 0; i < 3; i++) paging_write(hot + (i * PGSIZE), 0, algo);
    for(int i = 0; i < 5; i++) paging_write(cold + (i * PGSIZE), 0, algo);

    printf(1, "--- Program 3: Hot/Cold Access Patterns (Algo: %d) ---\n", algo);
    reset_paging_metrics();
    uint start_ticks = uptime();

    for(int k = 0; k < 50; k++) {
        for(int i = 0; i < 100; i++) {
            paging_read(hot + (0 * PGSIZE), algo);
            paging_read(hot + (1 * PGSIZE), algo);
            paging_read(hot + (2 * PGSIZE), algo);
        }
        for(int j = 0; j < 5; j++) {
            paging_read(cold + (j * PGSIZE), algo);
        }
    }
    uint end_ticks = uptime();
    
    paging_metrics();
    printf(1, "Execution Time: %d ticks\n", end_ticks - start_ticks);
    
    exit();
}