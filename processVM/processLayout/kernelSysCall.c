#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define FILENAME "test_file.txt"
#define BUFFER_SIZE 1024

void check_error(int result, const char *msg) {
    if (result == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

int main() {
    pid_t pid;
    int fd, status;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

    fd = open(FILENAME, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    check_error(fd, "open");

    const char *message = "Hello, Kernel Space!\n";
    bytes_written = write(fd, message, strlen(message));
    check_error(bytes_written, "write");
    printf("Wrote %zd bytes to file\n", bytes_written);

    close(fd);

    pid = fork();
    check_error(pid, "fork");

    if (pid == 0) {
        fd = open(FILENAME, O_RDONLY);
        check_error(fd, "open in child");

        bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
        check_error(bytes_read, "read");
        buffer[bytes_read] = '\0';

        printf("Child read: %s", buffer);
        close(fd);

        exit(EXIT_SUCCESS);
    } else {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        }

        if (unlink(FILENAME) == -1) {
            perror("unlink");
            exit(EXIT_FAILURE);
        }
        printf("File deleted\n");
    }

    return 0;
}
