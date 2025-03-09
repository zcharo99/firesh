#include "main.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

char* expand_variables(char *line) {
    static char expanded_line[1024];
    char *cursor = expanded_line;
    char *start = line;

    while (*start) {
        if (*start == '$') {
            start++;
            char var_name[256];
            char *var_cursor = var_name;

            // Extract the variable name
            while (*start && (isalnum(*start) || *start == '_')) {
                *var_cursor++ = *start++;
            }
            *var_cursor = '\0';

            // Get the value of the variable
            char *value = getenv(var_name);
            if (value) {
                cursor += sprintf(cursor, "%s", value);
            }
        } else {
            *cursor++ = *start++;
        }
    }
    *cursor = '\0';
    return expanded_line;
}