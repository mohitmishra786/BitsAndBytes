#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096

void print_memory_info(void *addr) {
    FILE *maps = fopen("/proc/self/maps", "r");
    if (maps == NULL) {
        perror("Failed to open /proc/self/maps");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), maps)) {
        unsigned long start, end;
        if (sscanf(line, "%lx-%lx", &start, &end) == 2) {
            if ((unsigned long)addr >= start && (unsigned long)addr < end) {
                printf("Address %p is in region: %s", addr, line);
                break;
            }
        }
    }

    fclose(maps);
}

int main() {
    // Allocate memory near the top of user space
    void *high_addr = mmap((void *)((1UL << 47) - PAGE_SIZE), PAGE_SIZE,
                           PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (high_addr == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    printf("Allocated memory at: %p\n", high_addr);
    print_memory_info(high_addr);

    // Try to access kernel space (this will fail)
    void *kernel_addr = (void *)0xffff800000000000; // Common start of kernel space on x86_64
    printf("Attempting to access kernel space at: %p\n", kernel_addr);
    print_memory_info(kernel_addr);

    // This will cause a segmentation fault
    *(int *)kernel_addr = 42;

    // Clean up (this won't be reached due to the segfault)
    munmap(high_addr, PAGE_SIZE);

    return 0;
}
