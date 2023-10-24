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

#define MAX_VARIABLE_NAME_LEN (64)
#define MAX_VARIABLE_VALUE_LEN (512)

typedef struct Variable {
  char name[MAX_VARIABLE_NAME_LEN];
  char value[MAX_VARIABLE_VALUE_LEN];
} Variable;

extern WColMap variable_map;
