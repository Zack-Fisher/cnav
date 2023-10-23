#include "main.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "command.h"
#include "complete.h"
#include "history.h"
#include "mode.h"
#include "oursignal.h"
#include "ps.h"
#include "run.h"
#include "terminal.h"
#include "whisper/colmap.h"

#define CTRL(ch) (ch & 0x1F)

#define CMP(cmp_ptr, lit) (strncmp(cmp_ptr, lit, strlen(lit)) == 0)

static ModeData const *curr_mode_data = NULL;

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

int handle_input_line(char *input) {
  if (input[0] == '_') { // parse the modeswitch command
    char *ptr = input + 1;
    return _try_switch_mode(ptr);
  } else { // parse a normal command based on the mode we're in.

    if (IS_MODE_INTERNAL(curr_mode_data->mode)) {
      // evaluate the expression in the normal bash shell in PATH.
      return parse_and_execute_command(input, strlen(input));
    } else if (IS_MODE_EXTERNAL(curr_mode_data->mode)) {
      return mode_run(curr_mode_data->mode, input);
    } else {
      printf("Invalid mode.\n");
      return INTERNAL_ERROR;
    }
  }
}

int run_script(const char *filepath) {
  int fd = open(filepath, O_RDONLY);
  if (fd == -1) {
    perror("open(): couldn't open the passed in script");
    return INTERNAL_ERROR;
  }

  struct stat file_stat;
  if (fstat(fd, &file_stat) == -1) {
    perror("fstat(): could not stat the passed in script");
    return INTERNAL_ERROR;
  }

  int len = file_stat.st_size;
  char script_buf[len];
  read(fd, script_buf, len);

  char *base_line = script_buf;
  char *next_newline = strchr(base_line, '\n');
  while (next_newline) {
    // replace the \n in the script with a \0.
    next_newline[0] = '\0';
    handle_input_line(base_line);
    base_line = next_newline + 1;
    next_newline = strchr(base_line, '\n');
  }
  handle_input_line(base_line); // the last dangling line.

  return 0;
}

int main(int argc, char *argv[]) {
  install_signal_handlers();

  char *starting_mode = NULL;
  char *script_filepaths_buffer[128];
  int script_filepaths_buffer_len = 0;
  { // arg parsing routine
    int i = 1;
    char *arg;
    while (1) {
#define NEXT()                                                                 \
  { arg = argv[i++]; }
      NEXT();
      if (i > argc) {
        break;
      }

      switch (arg[0]) {
      case '-': {
        switch (arg[1]) {
        case 'f': {
          NEXT();
          starting_mode = arg;
        } break;
        }
      } break;

      default: {
        // nothing else matched, so we're running this arg as a
        // script/filepath.
        script_filepaths_buffer[script_filepaths_buffer_len++] = arg;
      } break;
      }

#undef NEXT
    }
  }

  if (starting_mode && (_try_switch_mode(starting_mode) != 0)) {
    fprintf(stderr, "Invalid mode was passed, '%s'. Exiting...\n",
            starting_mode);
    return INTERNAL_ERROR;
  }

  // only try to move into the default "shell" mode if we have no passed -f
  // mode.
  if (!curr_mode_data && (_try_switch_mode("shell") != 0)) {
    fprintf(stderr, "Something is very wrong, 'shell' should be a valid "
                    "mode. Could not "
                    "switch mode, exiting...\n");
    return INTERNAL_ERROR;
  }

  if (script_filepaths_buffer_len > 0) {
    int err = 0;
    for (int i = 0; i < script_filepaths_buffer_len; i++) {
      err = run_script(script_filepaths_buffer[i]);
      if (err != 0) {
        fprintf(stderr,
                "Script '%s' failed with error code '%d', not running "
                "the rest.\n",
                script_filepaths_buffer[i], err);
      }
    }
    // all scripts pass, say nothing and exit.
    return 0;
  }

  {
    char *home = getenv("HOME");
    char fpath_buf[128];
    sprintf(fpath_buf, "%s%s", home ? home : "", "/.cnavrc");
    int err = run_script(fpath_buf);
    if (err != 0) {
      fprintf(stderr, "~/.cnavrc failed with '%d'.\n", err);
    }
  }

  // else, we're in an interactive shell.
  term_setup();

  char input[MAX_INPUT_LEN];
  while (1) {
    size_t len = 0;

#define PUSH_CHAR(ch)                                                          \
  {                                                                            \
    input[len++] = ch;                                                         \
    input[len] = '\0';                                                         \
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
    if (len > 0) {                                                             \
      PUTSTR("\b \b");                                                         \
      input[len] = '\0';                                                       \
      len--;                                                                   \
    }                                                                          \
  }

#define DELETE_INPUT()                                                         \
  {                                                                            \
    int total_len = len;                                                       \
    for (int i = 0; i < total_len; i++) {                                      \
      BACKSPACE();                                                             \
    }                                                                          \
  }

    // dump the command we're currently editing and replace it with the new
    // string.
#define REPLACE_CMD(string_to_replace)                                         \
  {                                                                            \
    DELETE_INPUT();                                                            \
    PUSH_STR(string_to_replace, strlen(string_to_replace));                    \
    PUTSTR(string_to_replace);                                                 \
  }

    PUTSTR(get_ps());

    // we can't just readline, we need to parse some escape codes
    // manually.
    char c;

#define READ() read(STDIN_FILENO, &c, 1);

    while (1) {
      READ();

      switch (c) {
      case '\177': {
        BACKSPACE();
      } break;

        // command submission.
      case '\n': {
        PUT('\n');
        PUSH_CHAR('\0');
        goto done_parsing_input;
      } break;

        // tab completion.
      case '\t': {
#define COMP_BUF_LEN (256)
        char comp_buf[COMP_BUF_LEN] = {0};
        if (completion_get(input, comp_buf, COMP_BUF_LEN)) {
          REPLACE_CMD(comp_buf);
        }
#undef COMP_BUF_LEN
      } break;

      case CTRL('w'): { // go back a word.
        BACKSPACE();
        while ((input[len - 1] != ' ') &&
               (len > 0)) { // backspace until we hit a ' '.
          BACKSPACE();
        }
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
              REPLACE_CMD(new_cmd_ptr);
            }
          } break;
          case 'B': {
            // down
            const char *new_cmd_ptr = history_forward();
            if (new_cmd_ptr) {
              REPLACE_CMD(new_cmd_ptr);
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

    // enter history no matter the type or mode.
    history_enter_command(input);

    int err = handle_input_line(input);
    set_ps_error_code(err);
  }

  return 0;
}
