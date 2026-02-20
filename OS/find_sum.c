#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

static void uitoa(int n, char *buf) {
  char tmp[16];
  int i = 0;
  if (n == 0) {
    buf[0] = '0';
    buf[1] = '\0';
    return;
  }
  while (n > 0 && i < sizeof(tmp)) {
    tmp[i++] = '0' + (n % 10);
    n /= 10;
  }
  int j = 0;
  while (i > 0) buf[j++] = tmp[--i];
  buf[j] = '\0';
}

static int sum_numbers_in_str(const char *s, int len) {
  int sum = 0;
  int num = 0;
  int in_number = 0;

  for (int i = 0; i < len; i++) {
    char c = s[i];
    if (c >= '0' && c <= '9') {
      num = num * 10 + (c - '0');
      in_number = 1;
    } else {
      if (in_number) {
        sum += num;
        num = 0;
        in_number = 0;
      }
    }
  }

  if (in_number)
    sum += num;

  return sum;
}

int
main(int argc, char *argv[])
{
  char buf[1024];
  int nread = 0;

  if (argc >= 2) {
    int i;
    for (i = 0; argv[1][i] && i < sizeof(buf); i++)
      buf[i] = argv[1][i];
    nread = i;
  } else {
    nread = read(0, buf, sizeof(buf));
    if (nread < 0) {
      printf(2, "find_sum: read error\n");
      exit();
    }
  }

  int sum = sum_numbers_in_str(buf, nread);

  unlink("result.txt");
  int fd = open("result.txt", O_CREATE | O_WRONLY);
  if (fd < 0) {
    printf(2, "find_sum: cannot open result.txt\n");
    exit();
  }

  char out[32];
  uitoa(sum, out);
  write(fd, out, strlen(out));
  write(fd, "\n", 1);
  close(fd);

  exit();
}