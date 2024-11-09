
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>

#define MAX_CMD_LENGTH 1024
#define MAX_ARGS 64
#define MAX_PATH 1024

void print_prompt() {
    char hostname[1024];
    char cwd[MAX_PATH];
    struct passwd *pw = getpwuid(getuid());

    gethostname(hostname, sizeof(hostname));
    getcwd(cwd, sizeof(cwd));

    printf("\033[1;32m%s@%s\033[0m:\033[1;34m%s\033[0m$ ", pw->pw_name, hostname, cwd);
    fflush(stdout);
}

void execute_command(char *args[], int input_fd, int output_fd) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("\033[1;31mError forking process\033[0m");
        exit(1);
    } else if (pid == 0) {
        // Child process
        if (input_fd != STDIN_FILENO) {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (output_fd != STDOUT_FILENO) {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        printf("\033[1;33mExecuting command: %s\033[0m\n", args[0]);
        execvp(args[0], args);
        fprintf(stderr, "\033[1;31mError executing command '%s': %s\033[0m\n", args[0], strerror(errno));
        exit(1);
    }
}

int main() {
    char input[MAX_CMD_LENGTH];
    char *commands[MAX_ARGS];
    int num_commands;
    int i;  /* Declare loop variable at the beginning */

    printf("\033[1;36mWelcome to MyShell!\033[0m\n");
    printf("Type 'exit' to quit the shell.\n\n");

    while (1) {
        print_prompt();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = 0; // Remove newline

        if (strcmp(input, "exit") == 0) {
            printf("\033[1;36mGoodbye!\033[0m\n");
            break;
        }

        // Split input into commands
        num_commands = 0;
        commands[num_commands] = strtok(input, "|");
        while (commands[num_commands] != NULL) {
            num_commands++;
            commands[num_commands] = strtok(NULL, "|");
        }

        int pipes[2];
        int input_fd = STDIN_FILENO;

        for (i = 0; i < num_commands; i++) {
            char *args[MAX_ARGS];
            int arg_count = 0;
            char *token;

            // Split command into arguments
            token = strtok(commands[i], " ");
            while (token != NULL) {
                args[arg_count++] = token;
                token = strtok(NULL, " ");
            }
            args[arg_count] = NULL;

            if (i < num_commands - 1) {
                if (pipe(pipes) == -1) {
                    perror("\033[1;31mError creating pipe\033[0m");
                    exit(1);
                }
            }

            int output_fd = (i == num_commands - 1) ? STDOUT_FILENO : pipes[1];

            execute_command(args, input_fd, output_fd);

            if (input_fd != STDIN_FILENO) {
                close(input_fd);
            }
            if (output_fd != STDOUT_FILENO) {
                close(output_fd);
            }

            input_fd = pipes[0];
        }

        // Wait for all child processes to finish
        int status;
        while (wait(&status) > 0) {
            if (WIFEXITED(status)) {
                printf("\033[1;32mChild process exited with status %d\033[0m\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("\033[1;31mChild process terminated by signal %d\033[0m\n", WTERMSIG(status));
            }
        }

        printf("\n");
    }

    return 0;
}
