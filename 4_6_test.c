#include "types.h"
#include "stat.h"
#include "user.h"

int main() {
    start();
    volatile int i;
    volatile int sum = 0;
    for(i = 0; i < 3; i++){
        psinfo();
        if(fork() == 0){
            volatile int j;
            for(j=0;j<10000000;j++) 
                    sum+=j;
            
            exit();
        }
    }

    for(i=0;i<3;i++) {
        int pid = wait();
    }
    end();
    exit();
}