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
    central_page_table.hit_count = 0;

    for(int i = 0; i < MAX_PTE; i++) {
        central_page_table.entries[i].valid = 0;
        central_page_table.entries[i].physical_addr = kalloc();
        
        if(central_page_table.entries[i].physical_addr != 0) {
            memset(central_page_table.entries[i].physical_addr, 0, PGSIZE);
        }

        central_page_table.entries[i].arrival_time = 0;
        central_page_table.entries[i].last_access = 0;
        central_page_table.entries[i].access_count = 0;
        central_page_table.entries[i].reference_bit = 0;
        central_page_table.entries[i].vpn = 0;
        central_page_table.entries[i].pid = -1;
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

int get_victim_lfu() {
    int victim = 0;
    uint min_freq = central_page_table.entries[0].access_count;
    for(int i = 1; i < 4; i++) {
        if(central_page_table.entries[i].access_count < min_freq) {
            min_freq = central_page_table.entries[i].access_count;
            victim = i;
        }
    }
    return victim;
}

int get_victim_clock() {
    static int hand = 0;
    while(1) {
        if (central_page_table.entries[hand].reference_bit == 0) {
            int victim = hand;
            hand = (hand + 1) % 4;
            return victim;
        }
        central_page_table.entries[hand].reference_bit = 0;
        hand = (hand + 1) % 4;
    }
}
int get_page_index(uint vaddr, int algo) {
    uint vpn = vaddr / PGSIZE;
    int pid = myproc()->pid;
    int target_idx = -1;

    for (int i = 0; i < 4; i++) {
        if (central_page_table.entries[i].valid && 
            central_page_table.entries[i].vpn == vpn && 
            central_page_table.entries[i].pid == pid) {
            
            central_page_table.hit_count++;
            central_page_table.entries[i].last_access = ticks;
            central_page_table.entries[i].access_count++;
            central_page_table.entries[i].reference_bit = 1; 
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
        if (algo == 0)      target_idx = get_victim_fifo();
        else if (algo == 1) target_idx = get_victim_lru();
        else if (algo == 2) target_idx = get_victim_lfu();
        else if (algo == 3) target_idx = get_victim_clock();
        else                target_idx = get_victim_fifo();
    }


    memmove(central_page_table.entries[target_idx].physical_addr, (void*)(vpn * PGSIZE), PGSIZE);
    central_page_table.entries[target_idx].vpn = vpn;
    central_page_table.entries[target_idx].pid = pid;
    central_page_table.entries[target_idx].valid = 1;     
    central_page_table.entries[target_idx].arrival_time = ticks;
    central_page_table.entries[target_idx].last_access = ticks;
    central_page_table.entries[target_idx].access_count = 1;
    central_page_table.entries[target_idx].reference_bit = 1;
    
    return target_idx;
}