// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)

  uint64 acq_count[NCPU];
  uint64 total_spins[NCPU];
};

struct rwlock {
  struct spinlock lk;
  int read_count;       
  int writing;          
  char *name;           
};

