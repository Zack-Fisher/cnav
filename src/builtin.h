#pragma once

#include "whisper/colmap.h"

#define ALIAS_FROM_LEN (64)
#define ALIAS_TO_LEN (256)
#define MAX_BUILTIN_NAME_LEN (64)

typedef struct Alias {
  char from[ALIAS_FROM_LEN];
  char to[ALIAS_TO_LEN];
} Alias;

extern WColMap alias_map;

// only export a hashmap of shell builtin commands.
extern WColMap builtin_map;

typedef int (*builtin_fn)(int, char *[]);

typedef struct Builtin {
  char name[MAX_BUILTIN_NAME_LEN];
  builtin_fn fn;
} Builtin;
