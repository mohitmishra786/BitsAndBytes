#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <assert.h>

bool is_pointer_valid(void* ptr, size_t size, bool write_access) {
    pid_t child = fork();
    
    if (child == 0) {
        // Child process
        if (write_access) {
            memset(ptr, 0, size);  // Attempt to write
        } else {
            char* buffer = malloc(size);
            if (buffer) {
                memcpy(buffer, ptr, size);  // Attempt to read
                free(buffer);
            }
        }
        exit(EXIT_SUCCESS);
    } else if (child > 0) {
        int status;
        pid_t result = waitpid(child, &status, 0);
        assert(result >= 0);
        return WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS;
    } else {
        perror("fork");
        return false;
    }
}

int main() {
    int* valid_ptr = malloc(sizeof(int));
    printf("Valid pointer check (write): %s\n", 
           is_pointer_valid(valid_ptr, sizeof(int), true) ? "PASS" : "FAIL");
    printf("Valid pointer check (read): %s\n", 
           is_pointer_valid(valid_ptr, sizeof(int), false) ? "PASS" : "FAIL");
    
    int* invalid_ptr = (int*)0xdeadbeef;
    printf("Invalid pointer check (write): %s\n", 
           is_pointer_valid(invalid_ptr, sizeof(int), true) ? "PASS" : "FAIL");
    printf("Invalid pointer check (read): %s\n", 
           is_pointer_valid(invalid_ptr, sizeof(int), false) ? "PASS" : "FAIL");
    
    free(valid_ptr);
    return 0;
}
