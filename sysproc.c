#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

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

int sys_paging_write(void) {
    char *ptr;
    int value;
    if (argptr(0, &ptr, sizeof(void*)) < 0 || argint(1, &value) < 0)
        return -1;

    acquire(&central_page_table.lock);
    
    central_page_table.total_accesses++;
    int idx = get_page_index((uint)ptr);
    
    uint offset = (uint)ptr % PGSIZE;
    
    *(int*)(central_page_table.entries[idx].physical_addr + offset) = value;

    central_page_table.entries[idx].last_access = ticks;
    central_page_table.entries[idx].access_count++;
    central_page_table.entries[idx].use_bit = 1;

    release(&central_page_table.lock);
    return 0; 
}

int sys_paging_read(void) {
    char *ptr;

    if (argptr(0, &ptr, sizeof(void*)) < 0)
        return -1;

    acquire(&central_page_table.lock); 
    
    central_page_table.total_accesses++;
    int idx = get_page_index((uint)ptr);
    
    uint offset = (uint)ptr % PGSIZE;
    
    int value = *(int*)(central_page_table.entries[idx].physical_addr + offset);

    central_page_table.entries[idx].last_access = ticks;
    central_page_table.entries[idx].access_count++;
    central_page_table.entries[idx].use_bit = 1;

    release(&central_page_table.lock);
    return value; 
}