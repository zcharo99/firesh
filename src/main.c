#include <stdio.h>
#include <stdlib.h>
#include "main.h"

void firesh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
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
