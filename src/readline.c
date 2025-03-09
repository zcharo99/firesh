#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define HISTORY_MAX 100
#define INPUT_BUFFER_SIZE 1024

// History management
static char *history[HISTORY_MAX];
static int history_count = 0;
static int history_index = 0;

// Terminal configuration
static struct termios orig_termios;

// Enable raw mode for terminal
static void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// Restore original terminal settings
static void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// Add command to history
static void add_to_history(char *line) {
    if (line[0] == '\0') return; // Don't add empty lines
    
    // If history is full, free the oldest entry
    if (history_count == HISTORY_MAX) {
        free(history[0]);
        for (int i = 0; i < history_count - 1; i++) {
            history[i] = history[i + 1];
        }
        history_count--;
    }
    
    // Add new entry
    history[history_count] = strdup(line);
    history_count++;
    history_index = history_count;
}

// Get command from history
static char *get_from_history(int offset) {
    if (history_count == 0) return NULL;
    
    if (offset >= 0 && offset < history_count) {
        return history[offset];
    }
    return NULL;
}

// Original read line function (kept for compatibility)
char *firesh_read_line(void)
{
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us

  if (getline(&line, &bufsize, stdin) == -1){
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // We recieved an EOF
    } else  {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }

  return line;
}

// Enhanced line reading function with arrow key support
char *firesh_read_line_with_editing() {
    enable_raw_mode();
    
    char *buffer = malloc(INPUT_BUFFER_SIZE);
    if (!buffer) {
        perror("firesh: allocation error");
        exit(EXIT_FAILURE);
    }
    
    int position = 0;
    int length = 0;
    int c;
    
    buffer[0] = '\0';
    
    while (1) {
        c = getchar();
        
        if (c == EOF || c == '\n') {
            buffer[length] = '\0';
            printf("\n");
            break;
        } else if (c == 127) { // Backspace
            if (position > 0) {
                if (position < length) {
                    // Delete character in the middle
                    memmove(&buffer[position-1], &buffer[position], length - position);
                }
                position--;
                length--;
                buffer[length] = '\0';
                
                // Redraw the line
                printf("\r\033[K"); // Clear line from cursor
                char *ps1 = getenv("PS1");
                if (!ps1) ps1 = "\\W \\$ ";
                char *expanded_ps1 = expand_ps1(ps1);
                printf("%s%s", expanded_ps1, buffer);
                
                // Move cursor to correct position
                printf("\r"); // Move to beginning
                printf("%s", expanded_ps1); // Print prompt
                for (int i = 0; i < position; i++) {
                    printf("%c", buffer[i]); // Move to position
                }
            }
        } else if (c == 27) { // Escape sequence
            // Read the next two characters
            char seq[3];
            seq[0] = getchar();
            seq[1] = getchar();
            seq[2] = '\0';
            
            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': // Up arrow
                        if (history_index > 0) {
                            history_index--;
                            char *hist_cmd = get_from_history(history_index);
                            if (hist_cmd) {
                                strncpy(buffer, hist_cmd, INPUT_BUFFER_SIZE - 1);
                                length = strlen(buffer);
                                position = length;
                                
                                // Redraw the line
                                printf("\r\033[K"); // Clear line from cursor
                                char *ps1 = getenv("PS1");
                                if (!ps1) ps1 = "\\W \\$ ";
                                char *expanded_ps1 = expand_ps1(ps1);
                                printf("%s%s", expanded_ps1, buffer);
                            }
                        }
                        break;
                    case 'B': // Down arrow
                        if (history_index < history_count) {
                            history_index++;
                            if (history_index == history_count) {
                                // Clear the line when reaching the end of history
                                buffer[0] = '\0';
                                length = 0;
                                position = 0;
                            } else {
                                char *hist_cmd = get_from_history(history_index);
                                if (hist_cmd) {
                                    strncpy(buffer, hist_cmd, INPUT_BUFFER_SIZE - 1);
                                    length = strlen(buffer);
                                    position = length;
                                }
                            }
                            
                            // Redraw the line
                            printf("\r\033[K"); // Clear line from cursor
                            char *ps1 = getenv("PS1");
                            if (!ps1) ps1 = "\\W \\$ ";
                            char *expanded_ps1 = expand_ps1(ps1);
                            printf("%s%s", expanded_ps1, buffer);
                        }
                        break;
                    case 'C': // Right arrow
                        if (position < length) {
                            position++;
                            printf("\033[C"); // Move cursor right
                        }
                        break;
                    case 'D': // Left arrow
                        if (position > 0) {
                            position--;
                            printf("\033[D"); // Move cursor left
                        }
                        break;
                }
            }
        } else if (isprint(c)) {
            if (length < INPUT_BUFFER_SIZE - 1) {
                if (position < length) {
                    // Insert character in the middle
                    memmove(&buffer[position+1], &buffer[position], length - position);
                }
                buffer[position] = c;
                position++;
                length++;
                buffer[length] = '\0';
                
                // Redraw the line
                printf("\r\033[K"); // Clear line from cursor
                char *ps1 = getenv("PS1");
                if (!ps1) ps1 = "\\W \\$ ";
                char *expanded_ps1 = expand_ps1(ps1);
                printf("%s%s", expanded_ps1, buffer);
                
                // Move cursor to correct position
                printf("\r"); // Move to beginning
                printf("%s", expanded_ps1); // Print prompt
                for (int i = 0; i < position; i++) {
                    printf("%c", buffer[i]); // Move to position
                }
            }
        }
    }
    
    disable_raw_mode();
    
    // Add to history if not empty
    if (length > 0) {
        add_to_history(buffer);
    }
    
    return buffer;
}

// Free history memory
void cleanup_history() {
    for (int i = 0; i < history_count; i++) {
        if (history[i] != NULL) {
            free(history[i]);
        }
    }
}