#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
  int n_children = 8; // Create 8 processes to ensure E-core is heavy
  int pid;

  printf(1, "--- Starting Load Balancing Test ---\n");

  for (int i = 0; i < n_children; i++) {
    pid = fork();
    if (pid == 0) {
      // --- CHILD PROCESS ---
      // Instead of a loop count, we wait for time to pass.
      // We want to live for ~200 ticks (approx 2 seconds)
      int start_time = uptime();
      while (uptime() - start_time < 200) {
         // Burn CPU cycles while waiting
         int x = 0;
         for(int k=0; k<1000; k++) x++; 
      }
      
      // Print only PID to minimize garbled text
      printf(1, "Child %d Done\n", getpid());
      exit();
    }
  }

  // --- PARENT PROCESS ---
  // Also stay alive so we count towards the load
  int start_time = uptime();
  while (uptime() - start_time < 200) {
     int x = 0;
     for(int k=0; k<1000; k++) x++; 
  }

  // Wait for children
  for (int i = 0; i < n_children; i++) {
    wait();
  }

  printf(1, "--- Test Finished ---\n");
  exit();
}