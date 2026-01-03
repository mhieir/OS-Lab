#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "sleeplock.h" 
#include "proc.h"
#include "paging.h"
#include "fs.h"
#include "file.h"
#include "fcntl.h"

struct Central_Page_Table central_page_table;
struct inode *swap_backing_store; // Pointer to the "Disk" file

// Helper to calculate file offset: simple (pid * large_offset) + vpn
// This ensures different processes don't overwrite each other in the swap file
static uint get_swap_offset(int pid, uint vpn) {
    return (pid * 1024 * PGSIZE) + (vpn * PGSIZE);
}

// Write a page from RAM to the Swap File
void swap_out(int victim_idx) {
    struct central_pte *e = &central_page_table.entries[victim_idx];
    if (!e->valid) return;

    uint offset = get_swap_offset(e->pid, e->vpn);
    
    // writei is the xv6 internal function to write to an inode
    begin_op();
    writei(swap_backing_store, e->physical_addr, offset, PGSIZE);
    end_op();
}

// Read a page from the Swap File into RAM
void swap_in(int target_idx, int pid, uint vpn) {
    struct central_pte *e = &central_page_table.entries[target_idx];
    uint offset = get_swap_offset(pid, vpn);

    // readi is the xv6 internal function to read from an inode
    begin_op();
    readi(swap_backing_store, e->physical_addr, offset, PGSIZE);
    end_op();
}

void paginginit(void) {
    initlock(&central_page_table.lock, "paging_table");
    central_page_table.hit_count = 0;

    // Initialize the "Disk" backing store
    // In a real xv6 environment, you'd create/open a specific swap file
    begin_op();
    swap_backing_store = namei("/SWAP_FILE");
    if(swap_backing_store == 0) {
        // If file doesn't exist, you'd create it here via itrunc or similar
        // For this snippet, assume the file is pre-allocated
    }
    end_op();

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
        swap_out(target_idx);
        central_page_table.entries[target_idx].valid = 0;
    }


    swap_in(target_idx, pid, vpn);

    central_page_table.entries[target_idx].vpn = vpn;
    central_page_table.entries[target_idx].pid = pid;
    central_page_table.entries[target_idx].valid = 1;      
    central_page_table.entries[target_idx].arrival_time = ticks;
    central_page_table.entries[target_idx].last_access = ticks;
    central_page_table.entries[target_idx].access_count = 1;
    central_page_table.entries[target_idx].reference_bit = 1;
    
    return target_idx;
}