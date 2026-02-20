#include "types.h"
#include "user.h"

int main(void) {
    int res = simple_arithmetic_syscall(5, 3);
    printf(1, "User: syscall result = %d\n", res);
    exit();
}