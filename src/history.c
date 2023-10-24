#include "history.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

History history = {0};

const char *history_back() {
  if (history.curr_position > 0) {
    return history.commands[--history.curr_position];
  } else {
    return NULL;
  }
}

const char *history_forward() {
  if (history.curr_position < history.last_command_position) { // Removed -1
    return history.commands[++history.curr_position];
  } else {
    return NULL;
  }
}

void history_enter_command(char const *cmd) {
  if (history.last_command_position < MAX_HISTORY_LEN) { // Removed -1
    strncpy(history.commands[history.last_command_position], cmd,
            MAX_INPUT_LEN - 1);
    history.commands[history.last_command_position][MAX_INPUT_LEN - 1] =
        '\0';                        // Null-terminate
    ++history.last_command_position; // Increment after
  } else {
    printf("Clearing history, it's full.\n");
    history_clear();
  }
  // reset the history state with every command that we enter.
  history.curr_position = history.last_command_position;
}

void history_clear() {
  memset(history.commands, 0,
         (history.last_command_position + 1) * MAX_INPUT_LEN); // Added +1
  history.last_command_position = 0;
  history.curr_position = 0; // last_command_position is now 0
}

void print_history() {
  printf("HISTORY:\n");
  for (int i = 0; i <= history.last_command_position - 1;
       i++) { // Changed loop condition to <= and -1
    printf("- %-4d: %s\n", i + 1, history.commands[i]);
  }
}
