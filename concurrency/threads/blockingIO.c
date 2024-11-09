#include <unistd.h>
#include <stdio.h>

int main() {
    char buffer[1024];
    ssize_t bytes_read;

    printf("Waiting for input…\n");
    bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));

    if (bytes_read > 0) {
        printf("Read %zd bytes: %.*s\n", bytes_read, (int)bytes_read, buffer);
    } else if (bytes_read == 0) {
        printf("End of file reached\n");
    } else {
        perror("read");
    }

    return 0;
}
