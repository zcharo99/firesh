#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    firesh_source_rc();

    do {
        char *user = getenv("USER");
        char hostname[256];
        gethostname(hostname, sizeof(hostname));

        char *ps1 = getenv("PS1");
        if (!ps1) {
            ps1 = "\\W \\$ ";
        }

        char ps1_final[512];
        snprintf(ps1_final, sizeof(ps1_final), ps1, user, hostname);

        if (getuid() == 0) {
            char *root_ps1 = strdup(ps1_final);
            root_ps1[strlen(root_ps1) - 1] = '#';
            ps1 = root_ps1;
        } else {
            ps1[strlen(ps1) - 1] = '$';
        }

        printf("%s", ps1);
        fflush(stdout);

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