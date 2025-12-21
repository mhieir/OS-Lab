
#include "stat.h"
#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if(test_sleeplock(1) < 0){
    printf(2, "Parent failed to acquire lock\n");
    exit();
  }
  printf(1, "Parent (PID %d) has acquired the lock.\n", getpid());
  int pid = fork();

  if(pid < 0){
    printf(2, "Fork failed\n");
    exit();
  }

  if(pid == 0){
    printf(1, "Child (PID %d) attempting to release Parent's lock...\n", getpid());
    test_sleeplock(2); 
    exit();
  } else {
    wait(); 
    test_sleeplock(2);
  }
  exit();
}