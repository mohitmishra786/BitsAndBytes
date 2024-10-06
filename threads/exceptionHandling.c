
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>

volatile sig_atomic_t interrupt_count = 0;
volatile sig_atomic_t segfault_count = 0;
static jmp_buf jmpbuf;

void handle_interrupt(int sig) {
    interrupt_count++;
    printf("Interrupt (SIGALRM) handled. Count: %d\n", interrupt_count);
}

void handle_segfault(int sig) {
    segfault_count++;
    printf("Segmentation Fault (SIGSEGV) handled. Count: %d\n", segfault_count);
    if (segfault_count >= 3) {
        printf("Too many segfaults. Exiting.\n");
        exit(1);
    }
    longjmp(jmpbuf, 1);
}

void trigger_interrupt() {
    struct itimerval timer;
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    printf("Setting up timer for interrupts...\n");
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer");
        return;
    }

    printf("Waiting for interrupts...\n");
    for (int i = 0; i < 3; i++) {
        pause();
    }

    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
}

void trigger_syscall() {
    char buffer[100];
    printf("Enter some text: ");
    fflush(stdout);

    ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("System call (read) triggered. Read %zd bytes: %s", bytes_read, buffer);
    } else {
        perror("read");
    }
}

void trigger_segfault() {
    int *ptr = NULL;
    if (setjmp(jmpbuf) == 0) {
        *ptr = 42;
    } else {
        printf("Recovered from segmentation fault\n");
    }
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = handle_interrupt;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction for SIGALRM");
        return 1;
    }

    sa.sa_handler = handle_segfault;
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction for SIGSEGV");
        return 1;
    }

    printf("1. Triggering interrupts (SIGALRM)\n");
    trigger_interrupt();

    printf("\n2. Triggering system call (read)\n");
    trigger_syscall();

    printf("\n3. Triggering segmentation faults (SIGSEGV)\n");
    for (int i = 0; i < 3; i++) {
        trigger_segfault();
    }

    printf("\nProgram completed successfully.\n");
    return 0;
}
