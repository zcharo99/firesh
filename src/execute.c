#include "main.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

// func for builtin commands
int firesh_cd(char **args);
int firesh_help(char **args);
int firesh_exit(char **args);
int firesh_export(char **args);

// array of builtin commands
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "export"
};

// associate it to the functions
int (*builtin_func[]) (char **) = {
  &firesh_cd,
  &firesh_help,
  &firesh_exit,
  &firesh_export
};

// number of builtins
int firesh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

// implementation for cd
int firesh_cd(char **args)
{
  if (args[1] == NULL) {
    char *home = getenv("HOME");
    if (home == NULL) {
      fprintf(stderr, "firesh: HOME not set\n");
    } else {
      if (chdir(home) != 0) {
        perror("firesh");
      }
    }
  } else {
    if (chdir(args[1]) != 0) {
      perror("firesh");
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

// implementation for export
int firesh_export(char **args)
{
  if (args[1] == NULL) {
    // If no arguments, list all environment variables
    extern char **environ;
    for (char **env = environ; *env != NULL; env++) {
      printf("%s\n", *env);
    }
    return 1;
  }

  // Handle export VAR=VALUE format
  char *equals_sign = strchr(args[1], '=');
  if (equals_sign) {
    // Split the string at the equals sign
    *equals_sign = '\0';
    char *name = args[1];
    char *value = equals_sign + 1;
    
    // Set the environment variable
    if (setenv(name, value, 1) != 0) {
      perror("firesh: export");
    }
  } else {
    // If no equals sign, just print the value of that variable
    char *value = getenv(args[1]);
    if (value) {
      printf("%s=%s\n", args[1], value);
    } else {
      printf("%s: not set\n", args[1]);
    }
  }
  
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
