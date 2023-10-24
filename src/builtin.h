#pragma once

#include "whisper/colmap.h"

#define ALIAS_SIDE_LEN (64)

typedef struct Alias {
  char from[ALIAS_SIDE_LEN];
  char to[ALIAS_SIDE_LEN];
} Alias;

extern WColMap alias_map;

// only export a hashmap of shell builtin commands.
extern WColMap builtin_map;

typedef int (*builtin_fn)(int, char *[]);

typedef struct Builtin {
  builtin_fn fn;
} Builtin;
