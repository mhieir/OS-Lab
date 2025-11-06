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
  printf(1, "%d\n", argc);
  int rc = make_duplicate(argv[1]);
  if(rc == 0){
    printf("OK: duplicated %s\n", argv[1]);
  }else if(rc == -1){
    printf("ERR: source not found: %s\n", argv[1]);
  }else{
    printf("ERR: generic failure for %s\n", argv[1]);
  }
}