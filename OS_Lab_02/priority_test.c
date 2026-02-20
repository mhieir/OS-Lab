// priority_test.c
#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int c1 = fork();
  if (c1 == 0) {
    set_priority_syscall(getpid(), 2); // low
    for (volatile int i = 0; i < 100000000; i++);
    printf(1, "child low done\n");
    exit();
  }

  int c2 = fork();
  if (c2 == 0) {
    set_priority_syscall(getpid(), 0); // high
    for (volatile int i = 0; i < 100000000; i++);
    printf(1, "child high done\n");
    exit();
  }

  wait();
  wait();
  exit();
}
