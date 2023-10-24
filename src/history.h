#pragma once

#include "main.h"

#define MAX_HISTORY_LEN (1000)

typedef struct History {
  char commands[MAX_HISTORY_LEN][MAX_INPUT_LEN];
  int last_command_position;
  int curr_position;
} History;

extern History history;

// these either return the pointer to the string or NULL if we've hit either end
// of the history.
const char *history_back();
const char *history_forward();

void history_enter_command(char const *cmd);
void history_clear();

void print_history();
