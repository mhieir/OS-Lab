// section - Examining the Steps of Executing a System Call at the Kernel Level Using GDB

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
void simple_pid(){
    int pid = getpid();
    printf("My PID: %d\n", pid);
  
}
int main() {
    simple_pid();
  exit();
}