#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

static sigjmp_buf jmpbuf;

void sigbus_handler(int signo) {
    siglongjmp(jmpbuf, 1);
}

void sigsegv_handler(int signo) {
    siglongjmp(jmpbuf, 2);
}

int main() {
    struct sigaction sa_bus, sa_segv;

    // Set up SIGBUS handler
    sa_bus.sa_handler = sigbus_handler;
    sigemptyset(&sa_bus.sa_mask);
    sa_bus.sa_flags = 0;
    sigaction(SIGBUS, &sa_bus, NULL);

    // Set up SIGSEGV handler
    sa_segv.sa_handler = sigsegv_handler;
    sigemptyset(&sa_segv.sa_mask);
    sa_segv.sa_flags = 0;
    sigaction(SIGSEGV, &sa_segv, NULL);

    int ret = sigsetjmp(jmpbuf, 1);
    if (ret == 0) {
        // Attempt to cause a SIGBUS error
        int fd = open("/dev/zero", O_RDONLY);
        if (fd == -1) {
            perror("open");
            return 1;
        }

        char *ptr = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, fd, 0);
        if (ptr == MAP_FAILED) {
            perror("mmap");
            close(fd);
            return 1;
        }

        close(fd);
        munmap(ptr, 4096);

        // Try to access unmapped memory (should cause SIGBUS)
        char c = *ptr;
        printf("This should not be printed: %c\n", c);
    } else if (ret == 1) {
        printf("SIGBUS error occurred\n");
    } else if (ret == 2) {
        printf("Segmentation fault (SIGSEGV) occurred\n");
    }

    // Attempt to cause a Segmentation Fault
    char *null_ptr = NULL;
    *null_ptr = 'x';  // This should cause a Segmentation Fault

    return 0;
}
