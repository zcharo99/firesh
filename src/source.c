#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "main.h"

void firesh_source_rc() {
    const char *rc_path = getenv("HOME"); // Get home directory
    if (!rc_path) return; // Fail silently if HOME is not set

    char rc_file[1024]; // Buffer size increased for safety
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
        } else {
            perror("Error creating ~/.firerc");  // Handle file creation error
            return;
        }
    }

    // Open ~/.firerc for reading
    FILE *file = fopen(rc_file, "r");
    if (!file) {
        perror("Error opening ~/.firerc");
        return; // If we can't open the file, return early
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, file) != -1) {
        // Check if line starts with PS1=
        if (strncmp(line, "PS1=", 4) == 0) {
            char *new_ps1 = line + 4; // Get value after "PS1="
            new_ps1[strcspn(new_ps1, "\n")] = 0; // Remove trailing newline

            // Optionally, validate or sanitize PS1 before setting it
            // For example, ensure there are no malicious characters
            if (new_ps1[0] == '\0') {
                fprintf(stderr, "Warning: Empty PS1 value in ~/.firerc\n");
                continue; // Skip empty PS1 values
            }

            setenv("PS1", new_ps1, 1); // Set as environment variable
        } else {
            // If it's not PS1, execute other commands in the rc file
            firesh_execute(firesh_split_line(line)); // Assuming firesh_execute and firesh_split_line are correctly implemented
        }
    }

    free(line);
    fclose(file);
}