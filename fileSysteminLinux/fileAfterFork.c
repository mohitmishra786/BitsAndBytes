#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {
    int fd = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // Child process
        const char *child_msg = "Hello from child!\n";
        write(fd, child_msg, 19);
        close(fd);
        exit(0);
    } else {
        // Parent process
        wait(NULL);  // Wait for child to finish
        const char *parent_msg = "Hello from parent!\n";
        write(fd, parent_msg, 20);
        close(fd);
    }

    return 0;
}
