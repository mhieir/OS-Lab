// Mutual exclusion spin locks.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

void
initlock(struct spinlock *lk, char *name)
{
  lk->name = name;
  lk->locked = 0;
  lk->cpu = 0;

  for(int i = 0; i < NCPU; i++){
    lk->acq_count[i] = 0;
    lk->total_spins[i] = 0;
  }
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
// Holding a lock for a long time may cause
// other CPUs to waste time spinning to acquire it.
// In spinlock.c

void
acquire(struct spinlock *lk)
{
  pushcli();
  if(holding(lk))
    panic("acquire");

  int spins = 0; // Initialize local counter

  // The loop check
  while(xchg(&lk->locked, 1) != 0) {
    spins++;
  }


  int id = cpuid();
  lk->acq_count[id]++;
  lk->total_spins[id] += spins; // ADD TO TOTAL

  __sync_synchronize();
  
  lk->cpu = mycpu();
  getcallerpcs(&lk, lk->pcs);
}

// Release the lock.
void
release(struct spinlock *lk)
{
  if(!holding(lk))
    panic("release");

  lk->pcs[0] = 0;
  lk->cpu = 0;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that all the stores in the critical
  // section are visible to other cores before the lock is released.
  // Both the C compiler and the hardware may re-order loads and
  // stores; __sync_synchronize() tells them both not to.
  __sync_synchronize();

  // Release the lock, equivalent to lk->locked = 0.
  // This code can't use a C assignment, since it might
  // not be atomic. A real OS would use C atomics here.
  asm volatile("movl $0, %0" : "+m" (lk->locked) : );

  popcli();
}

// Record the current call stack in pcs[] by following the %ebp chain.
void
getcallerpcs(void *v, uint pcs[])
{
  uint *ebp;
  int i;

  ebp = (uint*)v - 2;
  for(i = 0; i < 10; i++){
    if(ebp == 0 || ebp < (uint*)KERNBASE || ebp == (uint*)0xffffffff)
      break;
    pcs[i] = ebp[1];     // saved %eip
    ebp = (uint*)ebp[0]; // saved %ebp
  }
  for(; i < 10; i++)
    pcs[i] = 0;
}

// Check whether this cpu is holding the lock.
int
holding(struct spinlock *lock)
{
  int r;
  pushcli();
  r = lock->locked && lock->cpu == mycpu();
  popcli();
  return r;
}


// Pushcli/popcli are like cli/sti except that they are matched:
// it takes two popcli to undo two pushcli.  Also, if interrupts
// are off, then pushcli, popcli leaves them off.

void
pushcli(void)
{
  int eflags;

  eflags = readeflags();
  cli();
  if(mycpu()->ncli == 0)
    mycpu()->intena = eflags & FL_IF;
  mycpu()->ncli += 1;
}

void
popcli(void)
{
  if(readeflags()&FL_IF)
    panic("popcli - interruptible");
  if(--mycpu()->ncli < 0)
    panic("popcli");
  if(mycpu()->ncli == 0 && mycpu()->intena)
    sti();
}


void rwlock_init(struct rwlock *rw, char *name) {
  initlock(&rw->lk, "rwlock_internal");
  rw->name = name;
  rw->read_count = 0;
  rw->writing = 0;
}

void rwlock_acquire_read(struct rwlock *rw) {
  acquire(&rw->lk);
  while (rw->writing > 0) {
    sleep(rw, &rw->lk);
  }
  rw->read_count++;
  release(&rw->lk);
}

void rwlock_release_read(struct rwlock *rw) {
  acquire(&rw->lk);
  rw->read_count--;
  if (rw->read_count == 0) {
    wakeup(rw);
  }
  release(&rw->lk);
}

void rwlock_acquire_write(struct rwlock *rw) {
  acquire(&rw->lk);
  while (rw->writing > 0 || rw->read_count > 0) {
    sleep(rw, &rw->lk);
  }
  rw->writing = 1;
  release(&rw->lk);
}

void rwlock_release_write(struct rwlock *rw) {
  acquire(&rw->lk);
  rw->writing = 0;
  wakeup(rw);
  release(&rw->lk);
}

