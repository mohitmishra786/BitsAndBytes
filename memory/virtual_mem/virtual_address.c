#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int var = 42;
    printf("Parent PID: %d\n", getpid());
    printf("Parent var address: %p\n", &var);

    pid_t pid = fork();
    if (pid == 0) {
        printf("Child PID: %d\n", getpid());
        printf("Child var address: %p\n", &var);
        var = 84;
        printf("Child var value: %d\n", var);
        _exit(0);
    } else {
        wait(NULL);
        printf("Parent var value: %d\n", var);
    }
    return 0;
}
