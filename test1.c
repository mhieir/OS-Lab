#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096
// int main() {
//     char *p = sbrk(2 * PGSIZE);
//     // Align p to the next page boundary
//     char *addr = (char*)(((uint)p + PGSIZE - 1) & ~(PGSIZE - 1));

//     printf(1, "Testing for a Hit...\n");
    
//     paging_write(addr, 100); // 1st access: MISS (Table is empty)
//     paging_write(addr, 200); // 2nd access: HIT (VPN is already in table)
    
//     paging_metrics(); // Should show Hit Count: 1
//     exit();
// }
int main() {
    char *p = sbrk(12 * PGSIZE);
    if(p == (char*)-1){
    printf(1, "sbrk failed\n");
    exit();
    }
    printf(1, "--- Program 1: Scanning 10+ Pages ---\n");
    for(int i = 0; i < 11; i++) {
        char *addr = p + (i * PGSIZE);
        paging_write(addr, i * 10);
        printf(1, "Accessed Page %d\n", i);
    }
    
    paging_metrics();
    exit();
}