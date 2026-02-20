#include "types.h"
#include "stat.h"
#include "user.h"

int main() {

    volatile int i;
    volatile int sum = 0;
    for(i = 0; i < 5; i++){
        if(fork() == 0){
            volatile int j;
            for(j=0;j<10000000;j++) 
                    sum+=j;
            
            exit();
        }
    }

    for(i=0;i<5;i++) {

        int pid = wait();
        printf(1, "Child %d done\n", pid);
    }
    exit();
}