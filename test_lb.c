#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
  // Ensure we create enough load to trigger "My_Load >= P_Load + 3"
  int n_children = 8; 
  int pid;

  printf(1, "--- Starting Load Balancing Test ---\n");
  printf(1, "Parent is creating %d children...\n", n_children);

  for (int i = 0; i < n_children; i++) {
    pid = fork();
    if (pid == 0) {
      // --- CHILD PROCESS ---
      // use volatile so the compiler doesn't delete the loop
      volatile int x = 0; 
      int start_time = uptime();
      
      // Run for ~200 ticks (2 seconds)
      while (uptime() - start_time < 200) {
         x++; // Just burn CPU
      }
      
      // OPTIONAL: If you implemented the 'get_proc_info' syscall 
      // from Section 5.5, print your CPU ID here to prove you moved.
      // Otherwise, rely on the Kernel log.
      exit();
    }
  }

  // --- PARENT PROCESS ---
  // The parent must also stay busy so it contributes to the queue count
  volatile int y = 0;
  int start_time = uptime();
  while (uptime() - start_time < 200) {
     y++; 
  }

  // Wait for children to finish
  for (int i = 0; i < n_children; i++) {
    wait();
  }

  printf(1, "--- Test Finished ---\n");
  exit();
}