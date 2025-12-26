#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "plock.h"

struct plock global_plock;
struct plock_node node_pool[NPROC]; // Pre-allocated nodes

void plock_init(struct plock *pl) {
  initlock(&pl->lk, "plock");
  pl->locked = 0;
  pl->head = 0;
}

void plock_acquire(struct plock *pl, int priority) {
  acquire(&pl->lk);

  if (!pl->locked) {
    pl->locked = 1;
    release(&pl->lk);
    return;
  }

  // Lock is occupied, add current process to the queue
  struct proc *curproc = myproc();
  struct plock_node *new_node = &node_pool[curproc->pid % NPROC];
  new_node->p = curproc;
  new_node->priority = priority;
  new_node->next = pl->head;
  pl->head = new_node;

  // Sleep and release the internal spinlock
  // When we wake up, plock_release has already handed us the lock
  sleep(curproc, &pl->lk);
  
  release(&pl->lk);
}

void plock_release(struct plock *pl) {
  acquire(&pl->lk);

  if (pl->head == 0) {
    pl->locked = 0;
    release(&pl->lk);
    return;
  }

  // Find max priority node
  struct plock_node *prev = 0, *curr = pl->head;
  struct plock_node *max_prev = 0, *max_node = pl->head;

  struct plock_node *temp_prev = 0;
  struct plock_node *temp_curr = pl->head;

  while (temp_curr != 0) {
    if (temp_curr->priority > max_node->priority) {
      max_node = temp_curr;
      max_prev = temp_prev;
    }
    temp_prev = temp_curr;
    temp_curr = temp_curr->next;
  }

  // Remove max_node from list
  if (max_prev == 0) 
    pl->head = max_node->next;
  else 
    max_prev->next = max_node->next;

  // Direct hand-off: keep pl->locked = 1
  wakeup(max_node->p);

  release(&pl->lk);
}