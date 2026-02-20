#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
    int i, pid;
    printf(1, "Starting throughput measurement...\n");
    start();
    for (i = 0; i < 10; i++) {
        pid = fork();
        if (pid == 0) { 
            volatile int i;
            for (i = 0; i < 10000000; i++) {} 
                exit(); 
        }
    }
    for (i = 0; i < 10; i++) 
        wait();

    end();
    printf(1, "Measurement completed.\n");
    exit();
}