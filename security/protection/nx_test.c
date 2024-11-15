#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

// Function to demonstrate stack execution attempt
void test_stack_execution() {
    // Array to hold machine code (NOP sled followed by simple exit)
    unsigned char code[] = {
        0x90, 0x90, 0x90, 0x90,  // NOP sled
        0xb8, 0x01, 0x00, 0x00, 0x00,  // mov eax, 1
        0xbb, 0x00, 0x00, 0x00, 0x00,  // mov ebx, 0
        0xcd, 0x80                      // int 0x80
    };
    
    void (*func)() = (void(*)())code;
    
    printf("Attempting to execute code on stack at address: %p\n", (void*)code);
    func();  // This should fail with NX enabled
}

// Function to test executable heap
void test_executable_heap() {
    // Allocate executable memory
    void* executable_memory = mmap(NULL, 4096, 
                                 PROT_READ | PROT_WRITE | PROT_EXEC,
                                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (executable_memory == MAP_FAILED) {
        perror("mmap failed");
        return;
    }
    
    // Copy shellcode to executable memory
    unsigned char code[] = {
        0xb8, 0x01, 0x00, 0x00, 0x00,  // mov eax, 1
        0xbb, 0x00, 0x00, 0x00, 0x00,  // mov ebx, 0
        0xcd, 0x80                      // int 0x80
    };
    
    memcpy(executable_memory, code, sizeof(code));
    
    void (*func)() = (void(*)())executable_memory;
    printf("Executing code in allocated memory at address: %p\n", executable_memory);
    func();
    
    munmap(executable_memory, 4096);
}

int main() {
    printf("Testing stack execution protection:\n");
    printf("==================================\n\n");
    
    // Get page size
    long page_size = sysconf(_SC_PAGESIZE);
    printf("System page size: %ld bytes\n\n", page_size);
    
    // Print memory protection settings
    printf("Memory Protection Settings:\n");
    FILE* maps = fopen("/proc/self/maps", "r");
    if (maps) {
        char line[256];
        while (fgets(line, sizeof(line), maps)) {
            printf("%s", line);
        }
        fclose(maps);
    }
    
    printf("\nAttempting stack execution...\n");
    test_stack_execution();
    
    printf("\nAttempting execution in allocated memory...\n");
    test_executable_heap();
    
    return 0;
}