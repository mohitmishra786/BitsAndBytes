#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>

static jmp_buf jmpbuf;
static volatile sig_atomic_t sig_caught = 0;

void segv_handler(int signum) {
    sig_caught = 1;
    siglongjmp(jmpbuf, 1);
}

int is_pointer_valid(void* ptr, size_t size, int write) {
    struct sigaction sa, old_sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = segv_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NODEFER;
    
    if (sigaction(SIGSEGV, &sa, &old_sa) == -1) {
        perror("sigaction");
        return 0;
    }

    sig_caught = 0;
    if (sigsetjmp(jmpbuf, 1) == 0) {
        if (write) {
            volatile char temp = *(volatile char*)ptr;
            *(volatile char*)ptr = temp;
        } else {
            volatile char temp = *(volatile char*)ptr;
            (void)temp;
        }
    }

    sigaction(SIGSEGV, &old_sa, NULL);
    return !sig_caught;
}

int main() {
    int* valid_ptr = malloc(sizeof(int));
    printf("Valid pointer check (write): %s\n", 
           is_pointer_valid(valid_ptr, sizeof(int), 1) ? "PASS" : "FAIL");
    printf("Valid pointer check (read): %s\n", 
           is_pointer_valid(valid_ptr, sizeof(int), 0) ? "PASS" : "FAIL");
    
    int* invalid_ptr = (int*)0xdeadbeef;
    printf("Invalid pointer check (write): %s\n", 
           is_pointer_valid(invalid_ptr, sizeof(int), 1) ? "PASS" : "FAIL");
    printf("Invalid pointer check (read): %s\n", 
           is_pointer_valid(invalid_ptr, sizeof(int), 0) ? "PASS" : "FAIL");
    
    free(valid_ptr);
    return 0;
}
