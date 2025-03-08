#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIRESH_TOK_BUFSIZE 64
#define FIRESH_TOK_DELIM " \t\r\n\a"

char **firesh_split_line(char *line)
{
  int bufsize = FIRESH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "firesh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, FIRESH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += FIRESH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "firesh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, FIRESH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}
