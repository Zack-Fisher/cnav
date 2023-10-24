#include "command.h"

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtin.h"
#include "history.h"
#include "main.h"
#include "terminal.h"

#include "variables.h"

char last_arg_buf[128];

char tokenize_line_result[64][MAX_INPUT_LEN] = {0};

// Tokenize input into command and arguments
int tokenize_line(char const *line) {
  int argc = 0;
  int i = 0;
  int curr_arg_idx = 0;

  bool is_in_quotes = false;

  while (line[i]) {
    switch (line[i]) {
    case '\\': {
      // skip the space ahead of the \.
      // hello\ world is one token.
      i++;
      tokenize_line_result[argc][curr_arg_idx++] = line[i];
    } break;

    case '"': {
      is_in_quotes = !is_in_quotes;
    } break;

    case ' ': {
      if (!is_in_quotes) { // normal handler
        while (line[i + 1] == ' ') {
          i++;
        }
        // start appending to the next word, a raw space means word break.
        tokenize_line_result[argc++][curr_arg_idx++] = '\0';
        curr_arg_idx = 0;
      } else {
        tokenize_line_result[argc][curr_arg_idx++] =
            line[i]; // put the space through verbatim, it's treated like a
                     // normal character.
      }
    } break;

    default: {
      tokenize_line_result[argc][curr_arg_idx++] = line[i];
    } break;
    }

    i++;
  }
  tokenize_line_result[argc++][curr_arg_idx++] = '\0';
  return argc;
}

int cmd_expand(const char *input, int input_len, char *buf, int buf_len) {
  char *_ptr = buf;
  char *home = getenv("HOME");
  if (home == NULL) {
    home = ""; // Default to empty string if HOME is not set
  }

  int i = 0, len;

  bool is_in_single_quotes = false;

  while (i < input_len) {

#define GET_STUFF_UNTIL(future_pointer)                                        \
  int stuff_len = future_pointer - (input + i);                                \
  char stuff_buf[stuff_len + 1];                                               \
  strncpy(stuff_buf, &input[i], stuff_len);                                    \
  stuff_buf[stuff_len] = '\0';

    if (is_in_single_quotes) {
      switch (input[i]) {
      case '\'': {
        *_ptr = '\"';
        _ptr++;
        is_in_single_quotes = false;
      } break;

      default: { // everything is passed through verbatim, nothing happens in
                 // single quotes.
        *_ptr = input[i];
        _ptr++;
      } break;
      }
    } else {
      switch (input[i]) {
      case '\'': {
        *_ptr = '\"';
        _ptr++;
        is_in_single_quotes = true;
      } break;

      case '\\': {
        // this is an ugly hack i think?
        if (input[i + 1] != ' ') {
          i++;
          *_ptr = input[i]; // treat the metacharacter like a normal one.
          _ptr++;
        } else {
          *_ptr = input[i]; // keep around the backslash for word splitting in
                            // the tokenizer.
          _ptr++;
        }
      } break;

      case '!': {
        if (input[i + 1] == '!') {
          i++;
          // need room to go back.
          if (history.last_command_position > 0) {
            char const *last_cmd =
                history.commands[history.last_command_position - 1];
            _ptr += sprintf(_ptr, "%s", last_cmd);
          }
        } else {
          // get the number after the !, and expand that history entry.
          char digit_buf[10];
          int digit_buf_len = 0;
          while (isdigit(input[i + 1])) {
            digit_buf[digit_buf_len++] = input[i + 1];
            i++;
          }
          digit_buf[digit_buf_len++] = '\0';
          int history_idx = atoi(digit_buf) + 1;
          if (history_idx < MAX_HISTORY_LEN) {
            _ptr += sprintf(_ptr, "%s", history.commands[history_idx]);
          }
        }
      } break;

      case '~': {
        _ptr += sprintf(_ptr, "%s", home);
        break;
      }

        // for now, just list out the whole dir. don't do any smart globbing.
      case '*': {
        char cwd_buf[128];
        getcwd(cwd_buf, 128);
        DIR *d = opendir(cwd_buf);
        if (d) {
          struct dirent *de;
          while ((de = readdir(d))) {
            if ((strcmp(de->d_name, ".") == 0) ||
                (strcmp(de->d_name, "..") == 0)) {
              continue;
            }
            _ptr += sprintf(_ptr, "%s ", de->d_name);
          }
          closedir(d);
        } else {
          perror("opendir");
          fprintf(stderr, "Failed to glob '%s'.\n", cwd_buf);
        }
      } break;

      case '$': {
        // need to bump past $
        i++;
        char variable_buf[64];
        int variable_len = 0;
#define NOT(ch) (input[i] != ch)
        while (NOT(' ') && NOT(':') && NOT('/') && NOT('\0')) {
          variable_buf[variable_len++] = input[i++];
        }
        i--;
        variable_buf[variable_len] = '\0';
#undef NOT
        char *value = getenv(variable_buf);
        if (value) {
          _ptr += sprintf(_ptr, "%s", value);
        }
      } break;

      default: {
        char const *space = strchr(&input[i], ' ');
        if (!space) {
          space = input + input_len;
        }
        GET_STUFF_UNTIL(space);
        Alias *a = w_cm_get(&alias_map, stuff_buf);
        // make sure it's a real match, this way hashmap collisions aren't
        // nearly as much of a problem.
        //
        // printf("'%s', '%s'\n", stuff_buf, (a) ? a->from : "");
        if (a && (strcmp(stuff_buf, a->from) == 0)) {
          _ptr += sprintf(_ptr, "%s", a->to);
          i += stuff_len; // bump past the stuff.
        }

        // don't even need sprintf here.
        *_ptr = input[i];
        _ptr++;
      }
      }
    }

    i++;
  }

  *_ptr = '\0'; // Null-terminate the buffer

  return _ptr - buf;
}

// i hate dynalloc.
// just make it REALLY big and statically allocated and nobody will notice.
#define EXPAND_BUF_LEN (16 * 1024)
static char expand_buf[EXPAND_BUF_LEN];

int parse_and_execute_command(char const *input, int input_len) {
  if (input_len <= 0) {
    return 0;
  }

  {
    char const *last_space = strrchr(input, ' ');
    if (last_space) {
      last_space++;
    } else {
      last_space = input;
    }

    strncpy(last_arg_buf, last_space, 128);
  }

  const int expanded_length =
      cmd_expand(input, input_len, expand_buf, EXPAND_BUF_LEN);

  // enter history no matter the type or mode.
  history_enter_command(expand_buf);

  {
    char *space = strchr(expand_buf, ' ');
    if (!space) {
      space = expand_buf + expanded_length + 1;
    }
    int word_len = space - expand_buf;
    char word_buf[word_len + 1];
    strncpy(word_buf, expand_buf, word_len);
    word_buf[word_len] = '\0';

    // if there's no shell builtin for the first word, then execute the whole
    // string as a sh -c arg.
    Builtin const *b = w_cm_get(&builtin_map, word_buf);
    if (!b || (strncmp(b->name, word_buf, MAX_BUILTIN_NAME_LEN) != 0)) {
      char *argv[] = {"/bin/sh", "-c", expand_buf, NULL};
      return execute_command(3, argv);
    }
  }

  int argc = tokenize_line(expand_buf);
  char *argv[argc];
  for (int i = 0; i < argc; i++) {
    argv[i] = tokenize_line_result[i];
  }

  // try to execute the builtin from argv[0], or fallback on just running it
  // through sh.
  Builtin const *b = w_cm_get(&builtin_map, argv[0]);
  if (b && (strncmp(b->name, argv[0], MAX_BUILTIN_NAME_LEN) == 0) && b->fn) {
    return b->fn(argc, argv);
  } else {
    return INTERNAL_ERROR;
  }
}

pid_t child_pid = -1;

int execute_command(int argc, char *argv[]) {
  // else, execute the normal command.
  pid_t wpid;
  int status;

  // don't force weird terminal settings on every child process.
  term_restore();

  child_pid = fork();
  if (child_pid == 0) {
    // Child process
    if (execvp(argv[0], argv) == -1) {
      perror("Error executing command");
    }
    exit(EXIT_FAILURE);
  } else if (child_pid < 0) {
    perror("Fork failed");
  } else {
    // Parent process
    do {
      wpid = waitpid(child_pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  term_setup();
  child_pid = -1; // inform the rest of the program that the child has exited.

  return status;
}
