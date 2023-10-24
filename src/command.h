#pragma once

#include <sys/wait.h>

#include "main.h"
#include "variables.h"
#include "whisper/colmap.h"

#define EXECUTE_AND_COUNT(eac_argv)                                            \
  int execute_and_count_result;                                                \
  {                                                                            \
    int eac_argc = 0;                                                          \
    while (eac_argv[eac_argc++])                                               \
      ;                                                                        \
    execute_and_count_result = execute_command(eac_argc, eac_argv);            \
  }

// buffer for the $_ variable.
extern char last_arg_buf[MAX_VARIABLE_VALUE_LEN];

// -1 if a child process isn't active.
extern pid_t child_pid;

// static argv buffer.
extern char tokenize_line_result[64][MAX_INPUT_LEN];
// Tokenize input into command and arguments, store resulting argv into
// tokenize_line_result and return argc.
int tokenize_line(char const *line);

int cmd_expand(char const *input, int input_len, char *buf, int buf_len);
int parse_and_execute_command(char const *input, int input_len);
int execute_command(int argc, char *argv[]);
