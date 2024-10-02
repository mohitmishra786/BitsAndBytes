#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

int main() {
    pid_t pid = getpid();
    printf("Process ID (using getpid): %d\n", pid);

    pid_t direct_pid = syscall(SYS_getpid);
    printf("Process ID (using syscall): %d\n", direct_pid);

    unsigned long kernel_addr = 0xC0000000;  // Typical start of kernel space on 32-bit systems
    int *ptr = (int *)kernel_addr;
    
    printf("Attempting to read from kernel space...\n");
    int value = *ptr;  // This will cause a segmentation fault
    
    printf("This line will never be reached.\n");

    return 0;
}
