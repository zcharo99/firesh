#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "main.h"

void firesh_source_rc() {
    const char *rc_path = getenv("HOME"); 
    if (!rc_path) return; 

    char rc_file[1024]; 
    snprintf(rc_file, sizeof(rc_file), "%s/.firerc", rc_path);
    
    struct stat buffer;
    if (stat(rc_file, &buffer) != 0) {
        
        FILE *new_rc = fopen(rc_file, "w");
        if (new_rc) {
            fprintf(new_rc, "# \\W = current short dir\n");
            fprintf(new_rc, "# \\h = hostname\n");
            fprintf(new_rc, "# \\u = user\n");
            fprintf(new_rc, "# \\$ = root or normal user\n");
            fprintf(new_rc, "# \\w = full path but replace home with ~\n\n");
            fprintf(new_rc, "PS1=\\W \\$ \n"); 
            fclose(new_rc);
            
        } else {
            perror("Error creating ~/.firerc");  
            return;
        }
    }
    
    FILE *file = fopen(rc_file, "r");
    if (!file) {
        perror("Error opening ~/.firerc");
        return; 
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, file) != -1) {
        if (strncmp(line, "PS1=", 4) == 0) {
            char *new_ps1 = line + 4; 
            new_ps1[strcspn(new_ps1, "\n")] = 0; 
            
            if (new_ps1[0] == '\0') {
                fprintf(stderr, "Warning: Empty PS1 value in ~/.firerc\n");
                continue; 
            }

            setenv("PS1", new_ps1, 1); 
        } else {
            char *trimmed = line;
            while (*trimmed == ' ' || *trimmed == '\t') trimmed++; 
            
            if (*trimmed != '\0' && *trimmed != '#' && *trimmed != '\n') {
                trimmed[strcspn(trimmed, "\n")] = 0;
                
                if (strlen(trimmed) > 0) {
                    char **args = firesh_split_line(trimmed);
                    firesh_execute(args);
                    free(args); 
                }
            }
        }
    }
    free(line);
    fclose(file);
}