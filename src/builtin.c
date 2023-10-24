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

MAKE_WCOLMAP(alias_map, sizeof(Alias), 1021, {
  // mapping from -> to.
  INSERT("ls", "ls --color=auto");
  INSERT("grep", "grep -rin");
  INSERT("objdump", "objdump -M intel");
  INSERT("clip", "xclip -sel clip <");
  INSERT("mkdir", "mkdir -p");
  INSERT("grep", "grep --color=always -n");
  INSERT("less", "less -R");
  INSERT("sudo", "doas");
  INSERT("vg", "valgrind --leak-check=full -v --track-origins=yes "
               "--log-file=vg_logfile.out");
  INSERT("pinstall", "pip install --break-system-packages");
  INSERT("clone", "clone --depth=1");
  INSERT("chx", "sudo chmod +x");
  INSERT("menu", "rofi -dmenu -matching fuzzy");
  INSERT("press", "sleep 0.11 && xdotool key");
  INSERT("chzack", "sudo chown -R zack:zack");
  INSERT("cen64", "cen64 -is-viewer ~/Documents/n64/pifdata.bin");
});

#undef INSERT

int alias_builtin(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: alias <from>=<to>\n");
    return 1;
  } else {
    char *eq = strchr(argv[1], '=');
    if (eq) {
      eq[0] = '\0';
      if (w_cm_insert(&alias_map, argv[1], eq + 1) == NULL) {
        // returned NULL, something's already there/we failed to allocate.
        fprintf(stderr, "Hashmap collision between '%s' and '%s'.\n", argv[1],
                (char *)w_cm_get(&alias_map, argv[1]));
        return 1;
      }
      return 0;
    } else {
      return 1;
    }
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
      fprintf(stderr, "(Tried to cd into '%s')\n", argv[1]);
      return errno;
    }
    char *ls_argv[] = {"ls", "--color=auto", NULL};
    EXECUTE_AND_COUNT(ls_argv);
    return 0;
  }
}

int export_builtin(int argc, char *argv[]) {
#define USAGE(problem_lit)                                                     \
  fprintf(stderr,                                                              \
          "Usage: export <name>=<value>, set a variable. (" problem_lit ")\n")
  if (argc < 2) {
    USAGE("Didn't provide second arg.");
    return 1;
  } else {
    char *eq = strchr(argv[1], '=');
    if (eq) {
      eq[0] = '\0';
      w_cm_insert(&variable_map, argv[1], eq + 1);
      return 0;
    } else {
      USAGE("no equals sign found in second arg.");
      return 1;
    }
  }
#undef USAGE
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
  w_cm_insert(&builtin_map, "export", &(Builtin){.fn = export_builtin});
  w_cm_insert(&builtin_map, "alias", &(Builtin){.fn = alias_builtin});
});
