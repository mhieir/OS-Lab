#define MAX_PTE 4
struct central_pte {
    void *physical_addr; // Pointer to kalloc'd memory [cite: 14]
    uint vpn;            // Virtual Page Number [cite: 15]
    int pid;             // Owner PID [cite: 16]
    int valid;           // Valid bit [cite: 17]
    
    // Algorithm parameters [cite: 18, 19]
    uint last_access;    // For LRU
    uint access_count;   // For LFU
    int use_bit;         // For Clock
    uint arrival_time;   // For FIFO
};

struct Central_Page_Table {
    struct spinlock lock;
    struct central_pte entries[MAX_PTE];
    int hit_count;
    int total_accesses;
};