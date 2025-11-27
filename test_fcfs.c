#include "types.h"
#include "stat.h"
#include "user.h"

// A heavy loop to consume CPU time
void spin_cpu() {
  int i;
  int j;
  for (i = 0; i < 5000000; i++) {
    j = i * i; // Just burn CPU cycles
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
      // --- CHILD PROCESS ---
      // We are the child. We want to be CPU intensive.
      // The child created first (i=0) has the smallest ctime.
      
      spin_cpu(); 
      printf(1, "Child %d (Created #%d) finished execution.\n", getpid(), i + 1);
      exit();
    } else {
      // --- PARENT PROCESS ---
      // Sleep briefly to ensure the next child has a distinctly later 'ctime'
      sleep(10); 
    }
  }

  // Parent waits for all children
  for (int i = 0; i < n_children; i++) {
    wait();
  }

  printf(1, "Test finished.\n");
  exit();
}