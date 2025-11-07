#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int
main(int argc, char *argv[])
{
  if(argc != 2){
    printf(2, "usage: dup_test <path>\n");
    exit();
  }
  int rc = make_duplicate(argv[1]);
  if(rc == 0){
    printf(1, "OK: duplicated %s\n", argv[1]);
  }else if(rc == -1){
    printf(1, "ERR: source not found: %s\n", argv[1]);
  }else{
    printf(1, "failure for %s\n", argv[1]);
  }
  exit();
}