#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

void test_logic() {
  int fd;
  char buf[16];

  fd = open("testfile", O_CREATE | O_WRONLY);
  write(fd, "INITIAL", 7);
  close(fd);

  printf(2, "--- STARTING TEST ---\n");

  for(int i = 0; i < 3; i++) {
    if(fork() == 0) {
      int rfd = open("testfile", O_RDONLY);
      printf("User: Reader %d calling read()...\n", i+1);
      read(rfd, buf, 7); 
      printf("User: Reader %d finished reading: %s\n", i+1, buf);
      close(rfd);
      exit();
    }
  }

  if(fork() == 0) {
    int wfd = open("testfile", O_WRONLY);
    printf(2, "User: Writer calling write()...\n");
    write(wfd, "UPDATED", 7);
    printf(2, "User: Writer finished writing!\n");
    close(wfd);
    exit();
  }

  for(int i = 0; i < 4; i++) wait();
  printf(2, "--- TEST COMPLETE ---\n");
}

int main() {
  test_logic();
  exit();
}