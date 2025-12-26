// plock.h
struct plock_node {
  struct proc *p;
  int priority;
  struct plock_node *next;
};

struct plock {
  struct spinlock lk;
  int locked;
  struct plock_node *head;
};
// extern struct plock global_plock;