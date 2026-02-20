#include "types.h"
#include "stat.h"
#include "user.h"

void spin_cpu() {
  int i;
  int j;
  for (i = 0; i < 5000000; i++) {
    j = i * i;
  }
}

int main(int argc, char *argv[]) {
  int n_children = 5;
  int pid;

  printf(1, "Starting FCFS Creation Time Test...\n");

  for (int i = 0; i < n_children; i++) {
    pid = fork();
    if (pid < 0) {
      printf(1, "Fork failed\n");
    } else if (pid == 0) {
      spin_cpu(); 
      printf(1, "Child %d (Created #%d) finished execution.\n", getpid(), i + 1);
      exit();
    } else {
      sleep(10); 
    }
  }

  for (int i = 0; i < n_children; i++) {
    wait();
  }

  printf(1, "Test finished.\n");
  exit();
}