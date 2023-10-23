#pragma once

#include "whisper/colmap.h"
#define MODE_NAME_LEN (32)

typedef enum Mode {
  BEGIN_EXTERNAL_MODES = -1,
  C_MODE,
  NASM_MODE,

  // only waste one slot on the labeling convention.
  // we're using these Mode variants as indices into an array so they need to be
  // tightly packed.
  END_EXTERNAL_MODES,
  BEGIN_INTERNAL_MODES = END_EXTERNAL_MODES,

  SHELL_MODE,
  END_INTERNAL_MODES,

  MODE_COUNT,
} Mode;

typedef struct ModeData {
  char name[MODE_NAME_LEN];
  Mode mode;
} ModeData;

extern WColMap mode_data_map;

void print_valid_modes();

#define IS_MODE_INTERNAL(mode)                                                 \
  (mode > BEGIN_INTERNAL_MODES && mode < END_INTERNAL_MODES)

#define IS_MODE_EXTERNAL(mode)                                                 \
  (mode > BEGIN_EXTERNAL_MODES && mode < END_EXTERNAL_MODES)

#define IS_MODE_VALID(mode) (IS_MODE_EXTERNAL(mode) || IS_MODE_INTERNAL(mode))
