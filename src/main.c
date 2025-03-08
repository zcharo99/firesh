#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include "main.h"

void firesh_loop(void)
{
    char *line;
    char **args;
    int status;

    firesh_source_rc(); // Load rc file before interactive shell

    do {
        char *ps1 = getenv("PS1"); // Get PS1 from env
        if (!ps1) {
            ps1 = "\\W \\$ "; // Fallback to default PS1 format
        }

        // Expand PS1 dynamically
        char *expanded_ps1 = expand_ps1(ps1);

        printf("%s", expanded_ps1); // Print the expanded prompt
        fflush(stdout); // Ensure it prints immediately

        line = firesh_read_line();
        args = firesh_split_line(line);
        status = firesh_execute(args);

        free(line);
        free(args);
    } while (status);
}


int main(int argc, char **argv) {
  firesh_loop(); // Start the interactive shell loop
  return 0;
}