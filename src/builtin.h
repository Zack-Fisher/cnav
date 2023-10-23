#pragma once

#include "whisper/colmap.h"

// only export a hashmap of shell builtin commands.
extern WColMap builtin_map;

typedef int (*builtin_fn)(int, char *[]);

typedef struct Builtin {
  builtin_fn fn;
} Builtin;
