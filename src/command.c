#include "command.h"

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin.h"
#include "main.h"
#include "terminal.h"

#include "variables.h"

int cmd_expand(const char *input, int input_len, char *buf, int buf_len) {
  char *_ptr = buf;
  char *home = getenv("HOME");
  if (home == NULL) {
    home = ""; // Default to empty string if HOME is not set
  }

  int i = 0, len;

  while (i < input_len) {

#define GET_WORD()                                                             \
  char const *space = strchr(&input[i], ' ');                                  \
  if (!space) {                                                                \
    space = input + input_len;                                                 \
  }                                                                            \
  int word_len = space - (input + i);                                          \
  char word_buf[word_len + 1];                                                 \
  strncpy(word_buf, &input[i], word_len);                                      \
  word_buf[word_len] = '\0';

    switch (input[i]) {
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
      GET_WORD();
      Variable *v = w_cm_get(&variable_map, word_buf);
      if (v && (strcmp(word_buf, v->name) == 0)) {
        _ptr += sprintf(_ptr, "%s", v->value);
        i += word_len; // bump past the word.
      }
    } break;

    default: {
      GET_WORD();
      Alias *a = w_cm_get(&alias_map, word_buf);
      // make sure it's a real match, this way hashmap collisions aren't nearly
      // as much of a problem.
      //
      // printf("'%s', '%s'\n", word_buf, (a) ? a->from : "");
      if (a && (strcmp(word_buf, a->from) == 0)) {
        _ptr += sprintf(_ptr, "%s", a->to);
        i += word_len; // bump past the word.
      }

      // don't even need sprintf here.
      *_ptr = input[i];
      _ptr++;
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

  const int expanded_length =
      cmd_expand(input, input_len, expand_buf, EXPAND_BUF_LEN);

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
    if (!w_cm_get(&builtin_map, word_buf)) {
      char *argv[] = {"/bin/sh", "-c", expand_buf, NULL};
      return execute_command(3, argv);
    }
  }

  // Tokenize input into command and arguments
  char *token;
  int argc = 0;
  char *argv[MAX_INPUT_LEN];

  token = strtok(expand_buf, " ");
  while (token != NULL) {
    argv[argc] = token;
    argc++;
    token = strtok(NULL, " ");
  }
  argv[argc] = NULL; // don't forget to NULL term, we're using the syscall
                     // directly for exec.

  // try to execute the builtin from argv[0], or fallback on just running it
  // through sh.
  Builtin const *b = w_cm_get(&builtin_map, argv[0]);
  if (b && b->fn) {
    return b->fn(argc, argv);
  } else {
    return INTERNAL_ERROR;
  }
}

int execute_command(int argc, char *argv[]) {
  // else, execute the normal command.
  pid_t pid, wpid;
  int status;

  // don't force weird terminal settings on every child process.
  term_restore();

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(argv[0], argv) == -1) {
      perror("Error executing command");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("Fork failed");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  term_setup();

  return status;
}
