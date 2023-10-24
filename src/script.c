#include "script.h"
#include "command.h"
#include "main.h"
#include "ps.h"
#include "run.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static ModeData const *curr_mode_data = NULL;

int try_switch_mode(const char *key) {
  char key_copy[strlen(key) + 1];
  strcpy(key_copy, key);
  char *whitespace = strchr(key_copy, ' ');
  if (whitespace) {
    // strip whitespace from the key copy with a null terminator.
    whitespace[0] = '\0';
  }

  ModeData *md = w_cm_get(&mode_data_map, key_copy);
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
    fprintf(stderr, "Failed to switch mode to '%s'.\n", key_copy);
    print_valid_modes();
    return INTERNAL_ERROR;
  }
}

int setup_mode(char const *init_mode) {
  if (init_mode && (try_switch_mode(init_mode) != 0)) {
    fprintf(stderr, "Invalid mode was passed, '%s'.\n", init_mode);
    return INTERNAL_ERROR;
  }

  // only try to move into the default "shell" mode if we have no passed -f
  // mode.
  if (!curr_mode_data && (try_switch_mode("shell") != 0)) {
    fprintf(stderr, "Something is very wrong, 'shell' should be a valid "
                    "mode. Could not "
                    "switch mode.\n");
    return INTERNAL_ERROR;
  }

  return 0;
}

int run_script(const char *filepath) {
  setup_mode(
      NULL); // always enter the same, default mode. we don't want previous
             // executions messing with other, unrelated scripts.

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
  char script_buf[len + 1];
  read(fd, script_buf, len);
  script_buf[len] = '\0';

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

static int _handle_input_line(char *input) {
  if (input[0] == '_') { // parse the modeswitch command
    char *ptr = input + 1;
    return try_switch_mode(ptr);
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

int handle_input_line(char *input) {
  int err = _handle_input_line(input);
  set_ps_error_code(err);
  update_variables(err);
  return err;
}
