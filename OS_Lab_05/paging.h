#define MAX_PTE 4
struct central_pte {
    void *physical_addr;
    uint vpn;
    int pid;
    int valid;
    int dirty;
    
    uint last_access;    // For LRU
    uint access_count;   // For LFU
    int reference_bit;   // For Clock
    uint arrival_time;   // For FIFO
};

struct Central_Page_Table {
    struct spinlock lock;
    struct central_pte entries[MAX_PTE];
    int hit_count;
    int total_accesses;
};