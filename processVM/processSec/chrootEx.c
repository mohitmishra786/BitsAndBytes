#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <new_root>\n", argv[0]);
        return 1;
    }

    const char *new_root = argv[1];

    if (chroot(new_root) == -1) {
        fprintf(stderr, "Error changing root directory: %s\n", strerror(errno));
        return 1;
    }

    if (chdir("/") == -1) {
        fprintf(stderr, "Error changing to new root directory: %s\n", strerror(errno));
        return 1;
    }

    printf("Changed root to: %s\n", new_root);
    system("pwd");

    return 0;
}
