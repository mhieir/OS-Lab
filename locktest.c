#include "types.h"
#include "stat.h"
#include "user.h"


#define NCPU 8

void
print_scores(uint64 *scores)
{
  int i;
  printf(1, "--- Lock Contention Scores (Avg Spins/Acquire) ---\n");
  for(i = 0; i < NCPU; i++){
    printf(1, "CPU %d: %d\n", i, (int)scores[i]);
  }
  printf(1, "--------------------------------------------------\n");
}

int
main(int argc, char *argv[])
{
  uint64 scores[NCPU];
  int i, pid;
  int n_children = 4;
  int loops = 100000;

  printf(1, "Starting lock contention test on 'tickslock'...\n");

  if(getlockstat(scores) < 0){
    printf(1, "Error: getlockstat failed\n");
    exit();
  }
  print_scores(scores);

  printf(1, "Forking %d children to create contention...\n", n_children);

  for(i = 0; i < n_children; i++){
    pid = fork();
    if(pid < 0){
      printf(1, "Fork failed\n");
      exit();
    }

    if(pid == 0){
      for(int j = 0; j < loops; j++){
        uptime(); 
      }
      exit();
    }
  }
  for(i = 0; i < n_children; i++){
    wait();
  }
  printf(1, "All children finished.\n");
  if(getlockstat(scores) < 0){
    printf(1, "Error: getlockstat failed\n");
    exit();
  }
  print_scores(scores);

  exit();
}