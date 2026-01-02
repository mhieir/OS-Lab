#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096
int main() {
    char *hot = sbrk(3 * PGSIZE);  // 3 Hot Pages 
    char *cold = sbrk(5 * PGSIZE); // 5 Cold Pages 

    printf(1, "--- Program 3: Hot/Cold Access Patterns ---\n");
    for(int k = 0; k < 50; k++) { // Outer loop [cite: 60]
        for(int i = 0; i < 100; i++) { // Access Hot Pages [cite: 61]
            paging_read(hot + (0 * PGSIZE));
            paging_read(hot + (1 * PGSIZE));
            paging_read(hot + (2 * PGSIZE));
        }
        for(int j = 0; j < 5; j++) { // Access Cold Pages [cite: 65]
            paging_read(cold + (j * PGSIZE));
        }
    }
    
    paging_metrics();
    exit();
}