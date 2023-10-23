#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "command.h"
#include "history.h"
#include "mode.h"
#include "oursignal.h"
#include "ps.h"
#include "run.h"
#include "terminal.h"
#include "whisper/colmap.h"

#define CMP(cmp_ptr, lit) (strncmp(cmp_ptr, lit, strlen(lit)) == 0)

ModeData const *curr_mode_data = NULL;

int _try_switch_mode(const char *ptr) {
  ModeData *md = w_cm_get(&mode_data_map, ptr);
  if (md && IS_MODE_VALID(md->mode)) {
    if (curr_mode_data) {
      // cleanup the old mode, if it exists. the main execution modes should be
      // mutually exclusive.
      remove_ps_mode(curr_mode_data->name);
    }

    // then setup the new mode.
    curr_mode_data = md;
    add_ps_mode(curr_mode_data->name);
    return 0;
  } else {
    fprintf(stderr, "Failed to switch mode to '%s'.\n", ptr);
    print_valid_modes();
    return INTERNAL_ERROR;
  }
}

__attribute__((destructor)) void cleanup_main() {
  static int has_cleaned = 0;
  if (!has_cleaned) {
    term_restore();
    has_cleaned = 1;
  }
}

int main() {
  install_signal_handlers();
  term_setup();

  if (_try_switch_mode("shell") != 0) {
    // we need a valid mode for init. this is a fatal error.
    return INTERNAL_ERROR;
  }

  char input[MAX_INPUT_LEN];

  while (1) {
    size_t len = 0;

#define PUSH_CHAR(ch)                                                          \
  {                                                                            \
    input[len] = ch;                                                           \
    len++;                                                                     \
  }

#define PUSH_STR(str, len)                                                     \
  {                                                                            \
    for (int i = 0; i < len; i++) {                                            \
      PUSH_CHAR(str[i]);                                                       \
    }                                                                          \
  }

#define PUTSTR(lit)                                                            \
  {                                                                            \
    printf("%s", lit);                                                         \
    fflush(stdout);                                                            \
  }

#define PUT(ch)                                                                \
  {                                                                            \
    putchar(ch);                                                               \
    fflush(stdout);                                                            \
  }

    // \b only moves it back, we need to "delete" the character and then
    // move the cursor to the newly freed space.
#define BACKSPACE()                                                            \
  {                                                                            \
    PUTSTR("\b \b");                                                           \
    input[len] = '\0';                                                         \
    len--;                                                                     \
  }

#define DELETE_INPUT()                                                         \
  {                                                                            \
    int total_len = len;                                                       \
    for (int i = 0; i < total_len; i++) {                                      \
      BACKSPACE();                                                             \
    }                                                                          \
  }

    PUTSTR(get_ps());

    // we can't just readline, we need to parse some escape codes manually.
    char c;

#define READ() read(STDIN_FILENO, &c, 1);

    while (1) {
      READ();

      switch (c) {
      case '\177': {
        BACKSPACE();
      } break;

      case '\n': {
        PUT('\n');
        PUSH_CHAR('\0');
        goto done_parsing_input;
      } break;

      case 0x1B: {
        READ();
        switch (c) {
        case 0x5B: {
          // arrow keys
          READ();
          switch (c) {

            // history navigation keys
          case 'A': {
            // up
            const char *new_cmd_ptr = history_back();
            if (new_cmd_ptr) {
              DELETE_INPUT();
              PUSH_STR(new_cmd_ptr, strlen(new_cmd_ptr));
              PUTSTR(new_cmd_ptr);
            }
          } break;
          case 'B': {
            // down
            const char *new_cmd_ptr = history_forward();
            if (new_cmd_ptr) {
              DELETE_INPUT();
              PUSH_STR(new_cmd_ptr, strlen(new_cmd_ptr));
              PUTSTR(new_cmd_ptr);
            }
          } break;

          case 'C': {
            // right
          } break;
          case 'D': {
            // left
          } break;
          }
          break;
        }
        }
      } break;

      default: {
        // printf("%d\n", c);
        // fflush(stdout);
        PUT(c);
        PUSH_CHAR(c);
      } break;
      }
    }

#undef READ
#undef DELETE_INPUT
#undef BACKSPACE
#undef PUT
#undef PUTSTR
#undef PUSH_STR
#undef PUSH_CHAR

  done_parsing_input : {}

    // enter commands no matter the type or mode.
    history_enter_command(input);

    int err = INTERNAL_ERROR;
    if (input[0] == '_') { // parse the modeswitch command
      char *ptr = input + 1;
      err = _try_switch_mode(ptr);

    } else { // parse a normal command based on the mode we're in.

      if (IS_MODE_INTERNAL(curr_mode_data->mode)) {
        // evaluate the expression in the normal bash shell in PATH.
        err = parse_and_execute_command(input, strlen(input));
      } else if (IS_MODE_EXTERNAL(curr_mode_data->mode)) {
        err = mode_run(curr_mode_data->mode, input);
      } else {
        printf("Invalid mode.\n");
        err = INTERNAL_ERROR;
      }
    }

    // the command is done executing, handle the result and loop around.
    set_ps_error_code(err);
  }

  return 0;
}
