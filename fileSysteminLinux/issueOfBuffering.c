#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    FILE *fp = fopen("buffered.txt", "w");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }

    fprintf(fp, "Hello");  // This might be buffered

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // Child process
        fprintf(fp, " from child!\n");
        fclose(fp);
        exit(0);
    } else {
        // Parent process
        wait(NULL);
        fprintf(fp, " from parent!\n");
        fclose(fp);
    }

    return 0;
}
