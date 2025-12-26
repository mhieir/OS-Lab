#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "sleeplock.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


struct sleeplock testlock;
void
testlockinit(void)
{
  initsleeplock(&testlock, "test lock");
}

int
sys_test_sleeplock(void)
{
  int op;
  if(argint(0, &op) < 0) return -1;

  if(op == 1) { 
    acquiresleep(&testlock);
    return 0;
  } else if(op == 2) {
    releasesleep(&testlock);
    return 0;
  }
  return -1;
}

extern struct spinlock tickslock; 
int
sys_getlockstat(void)
{
  uint64 *score;

  if(argptr(0, (void*)&score, sizeof(uint64) * NCPU) < 0)
    return -1;

  struct spinlock *target_lock = &tickslock;

  uint64 k_scores[NCPU];
  for(int i = 0; i < NCPU; i++){
      if(target_lock->total_spins[i] != 0 && target_lock->acq_count[i] != 0)
      {
          k_scores[i] = (uint)target_lock->total_spins[i] / (uint)target_lock->acq_count[i];
      }
      else
      {
          k_scores[i] = 0;
      }
      //  k_scores[i]  = (uint)target_lock->total_spins[i];
  }

  if(copyout(myproc()->pgdir, (uint64)score, (void*)k_scores, sizeof(uint64) * NCPU) < 0)
    return -1;
  return 0;
}



struct rwlock test_lock;

int sys_rwlock_init(void) {
  rwlock_init(&test_lock, "test_rw_lock");
  return 0;
}

int sys_rwlock_acquire_read(void) {
  rwlock_acquire_read(&test_lock);
  return 0;
}

int sys_rwlock_release_read(void) {
  rwlock_release_read(&test_lock);
  return 0;
}

int sys_rwlock_acquire_write(void) {
  rwlock_acquire_write(&test_lock);
  return 0;
}

int sys_rwlock_release_write(void) {
  rwlock_release_write(&test_lock);
  return 0;
}

int sys_plock_acquire(void) {
  int priority;
  if(argint(0, &priority) < 0) return -1;
  plock_acquire(&global_plock, priority);
  return 0;
}

int sys_plock_release(void) {
  plock_release(&global_plock);
  return 0;
}