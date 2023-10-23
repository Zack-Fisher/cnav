#include "mode.h"
#include "whisper/array.h"
#include <stdio.h>

#define INSERT(mode_variant, lit)                                              \
  w_cm_insert(&mode_data_map, lit,                                             \
              &(ModeData){.name = lit, .mode = mode_variant})

#define MAP_SZ (509)

MAKE_WCOLMAP(mode_data_map, sizeof(ModeData), MAP_SZ, {
  INSERT(C_MODE, "c");
  INSERT(NASM_MODE, "nasm");
  INSERT(SHELL_MODE, "shell");
});

void print_valid_modes() {
  printf("Valid modes:\n");
  for (int i = 0; i < MAP_SZ; i++) {
    ModeData const *md = w_array_get(&mode_data_map, i);
    if (md) {
      printf("- %s\n", md->name);
    }
  }
}

#undef INSERT
