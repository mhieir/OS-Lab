struct rwlock {
  struct spinlock lk;   // Spinlock to protect the fields below
  int read_count;       // Number of active readers
  int writing;          // 1 if a writer is active, 0 otherwise
  char *name;           // Name of the lock
};