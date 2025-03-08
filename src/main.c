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

    firesh_source_rc(); 

    do {
        char *ps1 = getenv("PS1"); 
        if (!ps1) {
            ps1 = "\\W \\$ "; 
        }
        
        char *expanded_ps1 = expand_ps1(ps1);

        printf("%s", expanded_ps1);
        fflush(stdout); 

        line = firesh_read_line();
        args = firesh_split_line(line);
        status = firesh_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv) {
  firesh_loop();
  return 0;
}