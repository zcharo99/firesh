#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string.h>

char* expand_ps1(char *ps1) {
    static char expanded_ps1[512];
    char *cursor = expanded_ps1;
    char *user = getenv("USER");
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    char *home = getenv("HOME");

    char *last_folder = strrchr(cwd, '/');
    if (last_folder) {
        last_folder++;
    } else {
        last_folder = cwd;
    }

    for (int i = 0; ps1[i] != '\0'; i++) {
        if (ps1[i] == '\\' && ps1[i + 1] != '\0') {
            switch (ps1[i + 1]) {
                case 'u':
                    cursor += sprintf(cursor, "%s", user);
                    i++;
                    break;
                case 'h':
                    cursor += sprintf(cursor, "%s", hostname);
                    i++;
                    break;
                case 'W':
                    if (home && strcmp(cwd, home) == 0) {
                        cursor += sprintf(cursor, "~");
                    } else {
                        cursor += sprintf(cursor, "%s", last_folder);
                    }
                    i++;
                    break;
                case 'w':
                    if (home && strncmp(cwd, home, strlen(home)) == 0) {
                        if (strlen(home) == 1) {
                            cursor += sprintf(cursor, "~");
                        } else {
                            cursor += sprintf(cursor, "~%s", cwd + strlen(home));
                        }
                    } else {
                        cursor += sprintf(cursor, "%s", cwd);
                    }
                    i++;
                    break;
                case '$':
                    if (getuid() == 0) {
                        cursor += sprintf(cursor, "#");
                    } else {
                        cursor += sprintf(cursor, "$");
                    }
                    i++;
                    break;
                case 'n':
                    cursor += sprintf(cursor, "\n");
                    i++;
                    break;
                case '"':
                case '\'':
                    // Ignore quotes by skipping the backslash
                    *cursor++ = ps1[i+1];
                    i++;
                    break;
                default:
                    *cursor++ = ps1[i];
            }
        } else if (ps1[i] == '"' || ps1[i] == '\'') {
            // Ignore quotes without backslashes
            continue;
        } else {
            *cursor++ = ps1[i];
        }
    }
    *cursor = '\0';
    return expanded_ps1;
}