#include "types.h"
#include "stat.h"
#include "user.h"

// Busy work to keep CPU occupied
void burn_cpu() {
  volatile int x = 0;
  for (int i = 0; i < 30000000; i++) {
    x = x + 1;
  }
}

int main(int argc, char *argv[]) {
  int n_children = 6; // Enough processes to trigger "Load >= Min + 3"
  int pid;

  printf(1, "--- Starting Load Balancing Test ---\n");
  printf(1, "Creating %d processes on E-core (CPU 0)...\n", n_children);

  for (int i = 0; i < n_children; i++) {
    pid = fork();
    if (pid == 0) {
      // Child Process
      burn_cpu();
      printf(1, "Child %d finished.\n", getpid());
      exit();
    }
  }

  // Parent also keeps CPU 0 busy to ensure load is high
  burn_cpu();

  // Wait for all children
  for (int i = 0; i < n_children; i++) {
    wait();
  }

  printf(1, "--- Test Finished ---\n");
  exit();
}