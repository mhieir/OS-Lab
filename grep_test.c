#include "types.h"
#include "stat.h"
#include "user.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  char buf[128];
  int r = grep_syscall(argv[1], argv[2], buf, sizeof(buf));
  if (r < 0)
    printf(1, "Keyword not found or file missing\n");
  else
    printf(1, "Matched line: %s\n", buf);
  exit();
}
