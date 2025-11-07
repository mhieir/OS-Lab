#include "types.h"
#include "user.h"

int
main(void)
{
  int pid1 = fork();
  if(pid1 == 0){
    int pid11 = fork();
    if(pid11 == 0){
      sleep(50);
      exit();
    }
    wait();
    sleep(100);
    exit();
  }

  int pid2 = fork();
  if(pid2 == 0){
    sleep(120);
    exit();
  }

  sleep(20);
  show_process_family(pid1);
  show_process_family(getpid());

  wait();
  wait();
  exit();
}