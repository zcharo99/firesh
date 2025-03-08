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
            fprintf(new_rc, "# \\W = current short dir\n");
            fprintf(new_rc, "# \\h = hostname\n");
            fprintf(new_rc, "# \\u = user\n");
            fprintf(new_rc, "# \\$ = root or normal user\n\n");
            fprintf(new_rc, "PS1=\\W \\$ \n"); // Default dynamic prompt with newline
            fclose(new_rc);
            // Removed debug print: printf("Created default ~/.firerc\n");
        } else {
            perror("Error creating ~/.firerc");  // Keep this error message as it's important
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
            // Skip comments and empty lines
            char *trimmed = line;
            while (*trimmed == ' ' || *trimmed == '\t') trimmed++; // Skip leading whitespace
            
            // Skip empty lines and comments
            if (*trimmed != '\0' && *trimmed != '#' && *trimmed != '\n') {
                // Remove trailing newline
                trimmed[strcspn(trimmed, "\n")] = 0;
                
                // Only execute non-empty commands
                if (strlen(trimmed) > 0) {
                    char **args = firesh_split_line(trimmed);
                    firesh_execute(args);
                    free(args); // Free the allocated memory for args
                }
            }
        }
    }

    free(line);
    fclose(file);
}