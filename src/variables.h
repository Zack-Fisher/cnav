#pragma once

#include "whisper/colmap.h"

int insert_variable(char const *name, char const *value);

#define MAX_VARIABLE_NAME_LEN (64)
#define MAX_VARIABLE_VALUE_LEN (512)

typedef struct Variable {
  char name[MAX_VARIABLE_NAME_LEN];
  char value[MAX_VARIABLE_VALUE_LEN];
} Variable;

extern WColMap variable_map;
