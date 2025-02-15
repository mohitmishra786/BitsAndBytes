#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PAGE_SHIFT 12
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define PFN_MASK (((1ULL << 55) - 1) & ~((1ULL << PAGE_SHIFT) - 1))

int main() {
    int var;
    int fd;
    uint64_t entry, pfn;
    off_t offset;

    printf("Virtual address of var: %p\n", &var);

    fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    offset = (uintptr_t)&var / PAGE_SIZE * sizeof(entry);
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        close(fd);
        return 1;
    }

    if (read(fd, &entry, sizeof(entry)) != sizeof(entry)) {
        perror("read");
        close(fd);
        return 1;
    }

    close(fd);

    if ((entry & (1ULL << 63)) == 0) {
        printf("Page not present\n");
        return 1;
    }

    pfn = entry & PFN_MASK;
    printf("Physical address: 0x%lx\n", (pfn << PAGE_SHIFT) | ((uintptr_t)&var & (PAGE_SIZE - 1)));

    return 0;
}
