#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "proc.h"
#include "paging.h"

struct Central_Page_Table central_page_table;

// Initialize the table at boot
void paginginit(void) {
    initlock(&central_page_table.lock, "paging_table");
    for(int i = 0; i < MAX_PTE; i++) {
        central_page_table.entries[i].valid = 0; //[cite: 27]
        central_page_table.entries[i].physical_addr = kalloc(); // Pre-allocate [cite: 19]
    }
}

void paging_exit_cleanup(int pid) {
  acquire(&central_page_table.lock);
  for(int i = 0; i < 4; i++){
    if(central_page_table.entries[i].pid == pid) 
      central_page_table.entries[i].valid = 0;
  }
  release(&central_page_table.lock);
}