#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define FILENAME "test_rw.txt"

int
mystrlen(char *s)
{
  int n;
  for(n = 0; s[n]; n++);
  return n;
}

void reader(int id) {
  char buf[16];
  int fd;
  
  printf(1, "Reader %d: Calling read()...\n", id);
  
  fd = open(FILENAME, O_RDONLY);
  if(fd < 0) {
    printf(1, "Reader %d: Error opening file\n", id);
    exit();
  }
  
  read(fd, buf, sizeof(buf));
  printf(1, "Reader %d: Read data: %s\n", id, buf);
  
  close(fd);
  printf(1, "Reader %d: Done.\n", id);
  exit();
}

void writer() {
  int fd;
  char *data = "UPDATED_DATA";

  printf(1, "Writer: Attempting to write to file...\n");
  
  fd = open(FILENAME, O_CREATE | O_WRONLY);
  if(fd < 0) {
    printf(1, "Writer: Error opening file\n");
    exit();
  }
  
  write(fd, data, mystrlen(data));
  close(fd);
  
  printf(1, "Writer: File updated.\n");
  exit();
}

int main(void) {
  int fd;
  int i;

  fd = open(FILENAME, O_CREATE | O_WRONLY);
  write(fd, "INITIAL_DATA", 12);
  close(fd);

  printf(1, "--- Starting Reader-Writer Test ---\n");

  for(i = 0; i < 3; i++) {
    if(fork() == 0) {
      reader(i + 1);
    }
  }

  sleep(5);

  if(fork() == 0) {
    writer();
  }

  for(i = 0; i < 4; i++) {
    wait();
  }

  printf(1, "--- Test Complete ---\n");
  exit();
}