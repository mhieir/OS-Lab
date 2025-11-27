#include "types.h"
#include "stat.h"
#include "user.h"

void spin(int count) {
  int i;
  for(i = 0; i < count; i++) {
    asm("nop");
  }
}

int main(int argc, char *argv[]) {
  int i;
  int pids[5];

  printf(1, "Starting FCFS Test...\n");

  for (i = 0; i < 5; i++) {
    pids[i] = fork();

    if (pids[i] == 0) {
      spin(100000000);
      printf(1, "Child %d (Index %d) finished.\n", getpid(), i);
      exit();
    } else {
      sleep(1); 
    }
  }

  for (i = 0; i < 5; i++) {
    wait();
  }

  printf(1, "Test finished.\n");
  exit();
}