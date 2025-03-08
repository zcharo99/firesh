#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "main.h"

void firesh_source_rc() {
    const char *rc_path = getenv("HOME"); // Get home directory
    if (!rc_path) return; // Fail silently if HOME is not set

    char rc_file[256];
    snprintf(rc_file, sizeof(rc_file), "%s/.firerc", rc_path); // ~/.firerc

    // Check if ~/.firerc exists
    struct stat buffer;
    if (stat(rc_file, &buffer) != 0) {
        // File doesn't exist, create it with default PS1
        FILE *new_rc = fopen(rc_file, "w");
        if (new_rc) {
            fprintf(new_rc, "# \\W = current short dir\n# \\h = hostname\n # \\u = user\n# \\$ = root or normal user\n\nPS1=\\W \\$ "); // Default dynamic prompt
            fclose(new_rc);
            printf("Created default ~/.firerc\n");
        }
    }

    // Open ~/.firerc for reading
    FILE *file = fopen(rc_file, "r");
    if (!file) return; // Still failed? Just skip

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, file) != -1) {
        // Check if line starts with PS1=
        if (strncmp(line, "PS1=", 4) == 0) {
            char *new_ps1 = line + 4; // Get value after "PS1="
            new_ps1[strcspn(new_ps1, "\n")] = 0; // Remove trailing newline
            setenv("PS1", new_ps1, 1); // Set as environment variable
        } else {
            firesh_execute(firesh_split_line(line)); // Execute other commands
        }
    }

    free(line);
    fclose(file);
}