#ifndef MAIN_H
#define MAIN_H

int firesh_launch(char **args);
char **firesh_split_line(char *line);
char *firesh_read_line(void);
int firesh_execute(char **args);
void firesh_source_rc();
char *expand_ps1(char *ps1);
char *firesh_read_line_with_editing(void);
void cleanup_history(void);

#endif