#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

jmp_buf env;

void signal_handler(int sig) {
    printf("Caught signal %d\n", sig);
    longjmp(env, 1);
}

void test_protections() {
    // Test stack canary
    char buffer[16];
    printf("Testing stack canary:\n");
    printf("Buffer address: %p\n", (void*)buffer);
    
    // Test ASLR
    void* heap_ptr = malloc(16);
    printf("\nTesting ASLR:\n");
    printf("Heap allocation: %p\n", heap_ptr);
    printf("Function address: %p\n", (void*)&test_protections);
    
    // Test NX
    printf("\nTesting NX protection:\n");
    unsigned char shellcode[] = {
        0x90, 0x90, 0x90, 0x90,  // NOP sled
        0xc3                      // ret
    };
    
    if (!setjmp(env)) {
        void (*func)() = (void(*)())shellcode;
        func();
        printf("Stack execution succeeded (NX might be disabled)\n");
    } else {
        printf("Stack execution failed (NX is working)\n");
    }
    
    free(heap_ptr);
}

int main() {
    signal(SIGSEGV, signal_handler);
    
    printf("Protection Mechanism Test\n");
    printf("========================\n\n");
    
    // Print compilation flags
    printf("Compilation information:\n");
    #ifdef __SSP__
        printf("Stack protector: Enabled\n");
    #else
        printf("Stack protector: Disabled\n");
    #endif
    
    #ifdef __PIE__
        printf("Position Independent Executable: Yes\n");
    #else
        printf("Position Independent Executable: No\n");
    #endif
    
    // Print ASLR status
    FILE* aslr_status = fopen("/proc/sys/kernel/randomize_va_space", "r");
    if (aslr_status) {
        char status;
        fscanf(aslr_status, "%c", &status);
        printf("ASLR Status: %c (0=disabled, 1=partial, 2=full)\n", status);
        fclose(aslr_status);
    }
    
    printf("\nRunning protection tests:\n");
    printf("=========================\n");
    
    for (int i = 0; i < 3; i++) {
        printf("\nIteration %d:\n", i + 1);
        test_protections();
        sleep(1);
    }
    
    return 0;
}