#include "main.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

int firesh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    // Check if args[0] is NULL or empty
    if (args[0] == NULL || args[0][0] == '\0') {
        fprintf(stderr, "firesh: empty command\n");
        return 1;
    }

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            // If execvp fails, print a more descriptive error message
            if (errno == ENOENT) {
                fprintf(stderr, "firesh: %s: command not found\n", args[0]);
            } else {
                fprintf(stderr, "firesh: %s: %s\n", args[0], strerror(errno));
            }
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        // Error forking
        perror("firesh: fork error");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
            if (wpid == -1) {
                // If waitpid fails, print error and break
                perror("firesh: waitpid error");
                break;
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}