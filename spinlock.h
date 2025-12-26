// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
  uint64 acq_count[NCPU];
  uint64 total_spins[NCPU];
};

struct rwlock {
  struct spinlock lk;   // Spinlock to protect the fields below
  int read_count;       // Number of active readers
  int writing;          // 1 if a writer is active, 0 otherwise
  char *name;           // Name of the lock
};

