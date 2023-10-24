#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "command.h"
#include "history.h"

#include "whisper/colmap.h"

#define INSERT(from_lit, to_lit)                                               \
  {                                                                            \
    w_cm_insert(&alias_map, from_lit,                                          \
                &(Alias){.from = from_lit "\0", .to = to_lit "\0"});           \
  }

// feel free to define aliases at compile-time to speed things up a bit.

MAKE_WCOLMAP(alias_map, sizeof(Alias), 509, {
  // mapping from -> to.
  INSERT("ls", "ls --color=auto");
  INSERT("grep", "grep -rin");
  printf("%s\n", (char *)w_cm_get(&alias_map, "ls"));
});

#undef INSERT

int alias_builtin(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: alias <from>=<to>\n");
    return 1;
  } else {
    return 0;
  }
}

int cd_builtin(int argc, char *argv[]) {
  if (argc < 2) {
    printf("You must provide a path to cd into.\n");
    return 1;
  } else {
    if (chdir(argv[1]) == -1) {
      perror("chdir");
      return errno;
    }
    char *ls_argv[] = {"ls", NULL};
    EXECUTE_AND_COUNT(ls_argv);
    return 0;
  }
}

int exit_builtin(int argc, char *argv[]) {
  if (argc > 1) {
    exit(atoi(argv[1]));
  } else {
    exit(0);
  }
}

int history_builtin(int argc, char *argv[]) {
  if (argc > 1) {
    if (strncmp(argv[1], "-c", strlen("-c")) == 0) {
      return 0;
    }
  }
  print_history();
  return 0;
}

MAKE_WCOLMAP(builtin_map, sizeof(Builtin), 509, {
  w_cm_insert(&builtin_map, "cd", &(Builtin){.fn = cd_builtin});
  w_cm_insert(&builtin_map, "exit", &(Builtin){.fn = exit_builtin});
  w_cm_insert(&builtin_map, "history", &(Builtin){.fn = history_builtin});
});
