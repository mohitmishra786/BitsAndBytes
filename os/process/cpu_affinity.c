#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sched.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <cpu_number>\n", argv[0]);
        return 1;
    }

    int cpu = atoi(argv[1]);
    cpu_set_t set;

    CPU_ZERO(&set);
    CPU_SET(cpu, &set);

    if (sched_setaffinity(0, sizeof(set), &set) == -1) {
        fprintf(stderr, "Error setting CPU affinity: %s\n", strerror(errno));
        return 1;
    }

    printf("Set CPU affinity to CPU %d\n", cpu);

    for (long i = 0; i < 10000000000L; i++) {
        if (i % 1000000000 == 0) {
            printf("Still running on CPU %d\n", sched_getcpu());
        }
    }

    return 0;
}
