#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "main.h"

int firesh_cd(char **args)
{
    char cwd[1024];

    // Check current working directory before any action
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current directory: %s\n", cwd);  // Debug print
    } else {
        perror("getcwd");
    }

    // If no arguments are passed to `cd`, change to $HOME
    if (args[1] == NULL) {
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "firesh: HOME not set\n");
        } else {
            // Check if the HOME directory exists before trying to chdir
            if (access(home, F_OK) == 0) {
                printf("Changing to home directory: %s\n", home);  // Debug print
                if (chdir(home) != 0) {
                    perror("firesh");  // Handle chdir failure
                }
            } else {
                fprintf(stderr, "firesh: %s: No such directory\n", home);
            }
        }
    } else {
        // If there's an argument, try to change to that directory
        printf("Changing to directory: %s\n", args[1]);  // Debug print

        // Check if the directory exists before trying to change
        if (access(args[1], F_OK) == 0) {
            if (chdir(args[1]) != 0) {
                perror("firesh");  // Handle chdir failure
            }
        } else {
            fprintf(stderr, "firesh: %s: No such directory\n", args[1]);
        }
    }

    return 1;
}