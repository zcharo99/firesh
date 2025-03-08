#include "main.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

// func for builtin commands
int firesh_cd(char **args);
int firesh_help(char **args);
int firesh_exit(char **args);

// array of builtin commands
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

// associate it to the functions
int (*builtin_func[]) (char **) = {
  &firesh_cd,
  &firesh_help,
  &firesh_exit
};

// number of builtins
int firesh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

// implementation for cd
int firesh_cd(char **args)
{
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("Current directory: %s\n", cwd);  // Debug print
  } else {
    perror("getcwd");  // If getcwd fails
  }

  if (args[1] == NULL) {
    // No argument, get the HOME environment variable
    char *home = getenv("HOME");
    if (home == NULL) {
      fprintf(stderr, "firesh: HOME not set\n");
    } else {
      printf("Changing to home directory: %s\n", home);  // Debug print
      if (chdir(home) != 0) {
        perror("firesh");  // This will print the error message if chdir fails
      }
    }
  } else {
    // Change to the directory specified in args[1]
    printf("Changing to directory: %s\n", args[1]);  // Debug print
    if (chdir(args[1]) != 0) {
      perror("firesh");  // This will print the error message if chdir fails
    }
  }
  return 1;
}

// implementation for help
int firesh_help(char **args)
{
  int i;
  printf("    zChar's firesh\n");
  printf("    Type program names and arguments, and hit enter.\n");
  printf("    The following commands are built in:\n");

  for (i = 0; i < firesh_num_builtins(); i++) {
    printf("      %s\n", builtin_str[i]);
  }

  printf("Use the man command (if you have it installed) for information on other programs.\n");
  return 1;
}

// i think you get it
int firesh_exit(char **args)
{
    return 0;
}

// check if the command is builtin or from bin
int firesh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // return if the command is empty
    return 1;
  }

  // idk what this does tbh
  for (i = 0; i < firesh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return firesh_launch(args);
}
