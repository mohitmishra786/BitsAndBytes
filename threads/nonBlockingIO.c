#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

int main() {
    char buffer[1024];
    ssize_t bytes_read;
    int flags;

    flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    printf("Attempting to read…\n");
    bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));

    if (bytes_read > 0) {
        printf("Read %zd bytes: %.*s\n", bytes_read, (int)bytes_read, buffer);
    } else if (bytes_read == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("No data available right now\n");
        } else {
            perror("read");
        }
    } else {
        printf("End of file reached\n");
    }

    return 0;
}
