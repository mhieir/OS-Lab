// plock.h
struct plock_node {
  struct proc *p;
  int priority;
  struct plock_node *next;
};

struct plock {
  struct spinlock lk;      // Internal spinlock to protect plock data
  int locked;              // Is the lock held? (1 = yes, 0 = no)
  struct plock_node *head; // Head of the waiting queue
};
// extern struct plock global_plock;