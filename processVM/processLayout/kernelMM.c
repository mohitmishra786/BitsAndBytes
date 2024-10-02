#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>

#define PAGE_SIZE 4096
#define ALLOCATION_SIZE (100 * PAGE_SIZE)

void print_memory_info(const char *stage) {
    FILE *statm = fopen("/proc/self/statm", "r");
    if (statm == NULL) {
        perror("Failed to open /proc/self/statm");
        return;
    }

    long size, resident, share, text, lib, data, dt;
    if (fscanf(statm, "%ld %ld %ld %ld %ld %ld %ld", &size, &resident, &share, &text, &lib, &data, &dt) != 7) {
        fprintf(stderr, "Failed to read memory info\n");
        fclose(statm);
        return;
    }

    fclose(statm);

    printf("%s:\n", stage);
    printf("  Total program size: %ld pages\n", size);
    printf("  Resident set size: %ld pages\n", resident);
    printf("  Shared pages: %ld\n", share);
    printf("  Text (code) pages: %ld\n", text);
    printf("  Data/stack pages: %ld\n", data);
    printf("\n");
}

int main() {
    print_memory_info("Initial state");

    // Allocate memory
    void *ptr = mmap(NULL, ALLOCATION_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    print_memory_info("After allocation");

    // Touch the memory to force page allocation
    memset(ptr, 0, ALLOCATION_SIZE);

    print_memory_info("After touching memory");

    // Free the memory
    if (munmap(ptr, ALLOCATION_SIZE) == -1) {
        perror("munmap failed");
        return 1;
    }

    print_memory_info("After freeing memory");

    // Try to allocate memory in kernel space (this will fail)
    void *kernel_ptr = mmap((void *)0xffffffff80000000, PAGE_SIZE, PROT_READ | PROT_WRITE, 
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (kernel_ptr == MAP_FAILED) {
        printf("Failed to allocate in kernel space as expected. Error: %s\n", strerror(errno));
    } else {
        printf("Unexpectedly succeeded in allocating kernel space memory at %p\n", kernel_ptr);
        munmap(kernel_ptr, PAGE_SIZE);
    }

    return 0;
}
