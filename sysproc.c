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
#include "fs.h"
#include "file.h"
#include "fcntl.h"
#include "stat.h"


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

int
sys_simple_arithmetic_syscall(void)
{
    int a, b;
    a = myproc()->tf->ebx;
    b = myproc()->tf->ecx;

    int result = (a + b) * (a - b);
    cprintf("Kernel: calc || (%d+%d)*(%d-%d) = %d\n", a, b, a, b, result);
    return result;
}

int 
sys_show_process_family(void) 
{
  int pid;
  if(argint(0, &pid) < 0)
    return 1;
  return show_process_family_internal(pid);
}

int
sys_grep_syscall(void)
{
  char *keyword, *filename, *user_buffer;
  int buffer_size;

  if (argstr(0, &keyword) < 0 ||
      argstr(1, &filename) < 0 ||
      argptr(2, &user_buffer, sizeof(char*)) < 0 ||
      argint(3, &buffer_size) < 0)
    return -1;

  struct inode *ip;
  begin_op();
  ip = namei(filename);
  if (ip == 0) { end_op(); return -1; }

  ilock(ip);
  int n = ip->size;
  if (n <= 0) { iunlockput(ip); end_op(); return -1; }

  if (n >= PGSIZE) n = PGSIZE - 1;

  char *data = kalloc();
  if (!data) { iunlockput(ip); end_op(); return -1; }

  int r = readi(ip, data, 0, n);
  iunlockput(ip);
  end_op();
  if (r < 0) { kfree(data); return -1; }

  data[n] = 0; 
  char *line = data;
  for (int i = 0; i <= n; i++) {
    if (data[i] == '\n' || data[i] == 0) {
      char saved = data[i];
      data[i] = 0;
      int match = 0;
      for (char *p = line; *p; p++) {
        char *q = p, *k = keyword;
        while (*q && *k && *q == *k) { q++; k++; }
        if (*k == 0) { match = 1; break; }
      }

      if (match) {
        int len = 0; while (line[len] && len < buffer_size - 1) len++;
        if (copyout(myproc()->pgdir, (uint)user_buffer, line, len) < 0) {
          kfree(data);
          return -1;
        }
        char nul = 0;
        copyout(myproc()->pgdir, (uint)user_buffer + len, &nul, 1);
        kfree(data);
        return len;
      }

      data[i] = saved;
      line = &data[i + 1];
    }
  }

  kfree(data);
  return -1;
  
}


int
sys_set_priority_syscall(void)
{
  int pid, priority;
  if (argint(0, &pid) < 0 || argint(1, &priority) < 0)
    return -1;
  return setpriority(pid, priority);
}
