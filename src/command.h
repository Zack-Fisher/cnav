#pragma once

#include "whisper/colmap.h"

#define EXECUTE_AND_COUNT(eac_argv)                                            \
  int execute_and_count_result;                                                \
  {                                                                            \
    int eac_argc = 0;                                                          \
    while (eac_argv[eac_argc++])                                               \
      ;                                                                        \
    execute_and_count_result = execute_command(eac_argc, eac_argv);            \
  }

int cmd_expand(char const *input, int input_len, char *buf, int buf_len);
int parse_and_execute_command(char const *input, int input_len);
int execute_command(int argc, char *argv[]);
