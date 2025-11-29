#include "types.h"
#include "stat.h"
#include "user.h"

void spin(int id) {
  int i;
  int j;
  for(i = 0; i < 10000000; i++) {
    j = i * i;
  }
  printf(1, "Child %d (on CPU ?) finished.\n", id);
}
int main(int argc, char *argv[]) {
  int i;
  int children = 6;
  printf(1, "Starting Queue Isolation Test...\n");
  for(i = 0; i < children; i++) {
    int pid = fork();
    if(pid == 0) {
      spin(getpid());
      exit();
    }
  }
  for(i = 0; i < children; i++) {
    wait();
  }
  printf(1, "Test finished.\n");
  exit();
}