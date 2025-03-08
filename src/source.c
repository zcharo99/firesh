#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "main.h"

void firesh_source_rc() {
    const char *rc_path = getenv("HOME");
    if (!rc_path) return;

    char rc_file[256];
    snprintf(rc_file, sizeof(rc_file), "%s/.firerc", rc_path);

    struct stat buffer;
    if (stat(rc_file, &buffer) != 0) { 
        FILE *new_rc = fopen(rc_file, "w");
        if (new_rc) {
            fprintf(new_rc, "PS1='\\u@\\h \\W$ '\n");
            fclose(new_rc);
            printf("Created default ~/.firerc\n");
        }
    }

    FILE *file = fopen(rc_file, "r");
    if (!file) return;

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, file) != -1) {
        if (strncmp(line, "PS1=", 4) == 0) {
            char *new_ps1 = line + 4;
            new_ps1[strcspn(new_ps1, "\n")] = 0;
            setenv("PS1", new_ps1, 1);
        } else {
            firesh_execute(firesh_split_line(line));
        }
    }

    free(line);
    fclose(file);
}