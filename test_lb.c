#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
  int n_children = 8; 
  int pid;

  printf(1, "--- Starting Load Balancing Test ---\n");

  for (int i = 0; i < n_children; i++) {
    pid = fork();
    if (pid == 0) {
      // volatile forces the compiler to actually do the math
      volatile int x = 0; 
      int start_time = uptime();
      
      // Run for 200 ticks (approx 2 seconds)
      while (uptime() - start_time < 200) {
         // Heavy math to consume CPU
         for(int k=0; k<10000; k++) {
             x = x + 1;
         }
      }
      printf(1, "Child %d Done\n", getpid());
      exit();
    }
  }

  // Parent also stays busy
  volatile int y = 0;
  int start_time = uptime();
  while (uptime() - start_time < 200) {
     for(int k=0; k<10000; k++) {
         y = y + 1;
     }
  }

  for (int i = 0; i < n_children; i++) {
    wait();
  }

  printf(1, "--- Test Finished ---\n");
  exit();
}