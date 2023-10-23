#include "command.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin.h"
#include "main.h"
#include "terminal.h"

int parse_and_execute_command(char *input, int input_len) {
  if (input_len <= 0) {
    return 0;
  }

  char input_buf[input_len];
  strcpy(input_buf, input);

  // Tokenize input into command and arguments
  char *token;
  int argc = 0;
  char *argv[MAX_INPUT_LEN];

  token = strtok(input, " ");
  while (token != NULL) {
    argv[argc] = token;
    argc++;
    token = strtok(NULL, " ");
  }
  argv[argc] = NULL; // don't forget to NULL term, we're using the syscall
                     // directly for exec.

  // try to execute the builtin from argv[0], or fallback on just running it
  // through sh.
  Builtin *b = w_cm_get(&builtin_map, argv[0]);
  if (b && b->fn) {
    return b->fn(argc, argv);
  } else {
    char *argv[] = {"/bin/sh", "-c", input_buf, NULL};
    return execute_command(3, argv);
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
