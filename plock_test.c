#include "types.h"
#include "user.h"

void test_worker(int priority, int delay) {
  sleep(delay);
//   printf(1, "PID %d (priority: %d) attempting to acquire...\n", getpid(), priority);
  plock_acquire(priority);
  printf(1, "--- PID %d (priority: %d) ACQUIRED lock ---\n", getpid(), priority);
  sleep(50);
  plock_release();
//   printf(1, "PID %d (priority: %d) released lock.\n", getpid(), priority);
  sleep(10);
  exit();
}

int main() {
  int priorities[] = {10, 20, 30, 40, 50};
  
  printf(1, "Starting Plock Priority Test...\n");

  if (fork() == 0) {
    plock_acquire(5);
    printf(1, "Initial Process (Pri: 5) holds lock. Others will queue.\n");
    sleep(100); 
    plock_release();
    exit();
  }

  sleep(20);

  for (int i = 0; i < 5; i++) {
    if (fork() == 0) {
      test_worker(priorities[i], 10);
    }
  }

  for (int i = 0; i < 6; i++) wait();
  
  printf(1, "Test complete.\n");
  exit();
}