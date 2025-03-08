#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

char* expand_ps1(char *ps1) {
    static char expanded_ps1[512];
    char *cursor = expanded_ps1;
    char *user = getenv("USER");
    char hostname[256];
    gethostname(hostname, sizeof(hostname)); // Get hostname
    char cwd[256];
    getcwd(cwd, sizeof(cwd)); // Get current directory

    for (int i = 0; ps1[i] != '\0'; i++) {
        if (ps1[i] == '\\' && ps1[i + 1] != '\0') {
            switch (ps1[i + 1]) {
                case 'u':
                    cursor += sprintf(cursor, "%s", user);  // Insert username
                    i++; // Skip over 'u'
                    break;
                case 'h':
                    cursor += sprintf(cursor, "%s", hostname);  // Insert hostname
                    i++; // Skip over 'h'
                    break;
                case 'W':
                    cursor += sprintf(cursor, "%s", cwd);  // Insert current directory
                    i++; // Skip over 'W'
                    break;
                case '$':
                    if (getuid() == 0) {
                        cursor += sprintf(cursor, "#");  // Root user, use #
                    } else {
                        cursor += sprintf(cursor, "$");  // Normal user, use $
                    }
                    i++; // Skip over '$'
                    break;
                default:
                    *cursor++ = ps1[i];  // Just copy the character if no match
            }
        } else {
            *cursor++ = ps1[i];  // Copy the character if no escape
        }
    }
    *cursor = '\0'; // Null-terminate the string
    return expanded_ps1;
}