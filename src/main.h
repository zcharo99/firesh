#include <stdio.h>

int firesh_launch(char **args);
char **firesh_split_line(char *line);
char *firesh_read_line(void);
int firesh_execute(char **args);
void firesh_source_rc();
char *expand_ps1(char *ps1);