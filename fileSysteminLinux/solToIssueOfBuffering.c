#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {
    int fd = open("unbuffered.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    write(fd, "Hello", 5);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // Child process
        write(fd, " from child!\n", 13);
        close(fd);
        exit(0);
    } else {
        // Parent process
        wait(NULL);
        write(fd, " from parent!\n", 14);
        close(fd);
    }

    return 0;
}
