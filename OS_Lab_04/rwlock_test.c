#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define FILENAME "test_rw.txt"

void reader(int id) {
  char buf[16];
  int fd;
  
  rwlock_acquire_read();
  
  printf(1, "R%d: acquired-lock\n", id);
  
  fd = open(FILENAME, O_RDONLY);
  if(fd < 0) {
    printf(1, "R%d: open error\n", id);
    rwlock_release_read();
    exit();
  }
  
  read(fd, buf, sizeof(buf));
  printf(1, "R%d: read: {%s}\n", id, buf);
  
  sleep(50); 
  
  close(fd);
  printf(1, "R%d: release\n", id);
  
  rwlock_release_read();
  exit();
}

void writer() {
  int fd;
  char *data = "UPDATED_DATA";

  printf(1, "W: waiting\n");
  rwlock_acquire_write();
  printf(1, "W: acquired-lock\n");
  
  fd = open(FILENAME, O_CREATE | O_WRONLY);
  if(fd < 0) {
    printf(1, "W: Open error\n");
    rwlock_release_write();
    exit();
  }
  
  write(fd, data, 12);
  sleep(20); 
  close(fd);
  
  printf(1, "W: finish\n");
  rwlock_release_write();
  exit();
}

int main() {
  int fd;
  int i;

  rwlock_init();

  fd = open(FILENAME, O_CREATE | O_WRONLY);
  write(fd, "INITIAL_DATA", 12);
  close(fd);

  for(i = 0; i < 3; i++) {
    if(fork() == 0) {
      reader(i + 1);
    }
  }

  if(fork() == 0) {
    writer();
  }

  for(i = 0; i < 4; i++) {
    wait();
  }

  exit();
}