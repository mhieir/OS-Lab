#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "proc.h"
#include "paging.h"

struct Central_Page_Table central_page_table;

void paginginit(void) {
    initlock(&central_page_table.lock, "paging_table");
    for(int i = 0; i < MAX_PTE; i++) {
        central_page_table.entries[i].valid = 0;
        central_page_table.entries[i].physical_addr = kalloc();
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

int get_victim_fifo() {
    int victim = 0;
    uint oldest = central_page_table.entries[0].arrival_time;
    for(int i = 1; i < 4; i++) {
        if(central_page_table.entries[i].arrival_time < oldest) {
            oldest = central_page_table.entries[i].arrival_time;
            victim = i;
        }
    }
    return victim;
}

int get_victim_lru() {
    int victim = 0;
    uint least_recent = central_page_table.entries[0].last_access;
    for(int i = 1; i < 4; i++) {
        if(central_page_table.entries[i].last_access < least_recent) {
            least_recent = central_page_table.entries[i].last_access;
            victim = i;
        }
    }
    return victim;
}

int get_page_index(uint vaddr) {
    uint vpn = vaddr / PGSIZE;
    int pid = myproc()->pid;
    int target_idx = -1;

        for (int i = 0; i < 4; i++) {
        if (central_page_table.entries[i].valid && 
            central_page_table.entries[i].vpn == vpn && 
            central_page_table.entries[i].pid == pid) {
            central_page_table.hit_count++;
            return i;
        }
    }

        for (int i = 0; i < 4; i++) {
        if (!central_page_table.entries[i].valid) {
            target_idx = i;
            break;
        }
    }

        if (target_idx == -1) {
                target_idx = 0;
        uint earliest = central_page_table.entries[0].arrival_time;
        for (int i = 1; i < 4; i++) {
            if (central_page_table.entries[i].arrival_time < earliest) {
                earliest = central_page_table.entries[i].arrival_time;
                target_idx = i;
            }
        }
    }

            memmove(central_page_table.entries[target_idx].physical_addr, (void*)(vpn * PGSIZE), PGSIZE);
    central_page_table.entries[target_idx].vpn = vpn;
    central_page_table.entries[target_idx].pid = pid;
    central_page_table.entries[target_idx].valid = 1;     central_page_table.entries[target_idx].arrival_time = ticks;     
    return target_idx;
}