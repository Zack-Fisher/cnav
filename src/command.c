#include "command.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin.h"
#include "main.h"
#include "terminal.h"

// expand input into buf.
int cmd_expand(char const *input, int input_len, char *buf, int buf_len) {
  char *_ptr = buf;
  int i = 0;
  while (true) {
    if (i >= input_len) {
      break;
    }

    switch (input[i]) {
    case '~': {
      _ptr += sprintf(_ptr, "%s", getenv("HOME"));
    } break;

    default: {
      _ptr += sprintf(_ptr, "%c", input[i]);
    } break;
    }

    i++;
  }
  return _ptr - buf;
}

int parse_and_execute_command(char const *input, int input_len) {
  if (input_len <= 0) {
    return 0;
  }

  char expand_buf[input_len * 4];
  const int expanded_length =
      cmd_expand(input, input_len, expand_buf, input_len * 4);
  // printf("raw input:\n%s\nexpanded:\n%s\n", input, expand_buf);

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
