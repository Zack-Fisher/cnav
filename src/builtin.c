#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "command.h"
#include "history.h"
#include "variables.h"

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
  INSERT("grep", "grep -n --color=auto");
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
      setenv(argv[1], eq + 1, true);
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

int testargs_builtin(int argc, char *argv[]) {
  for (int i = 0; i < argc; i++) {
    printf("[%d]: %s\n", i, argv[i]);
  }
  return 0;
}

#include <dirent.h>

bool is_in_dir(char const *filename, char const *dirpath) {
  DIR *d = opendir(dirpath);
  if (d) {
    struct dirent *de;
    while ((de = readdir(d))) {
      if (strcmp(de->d_name, filename) == 0) {
        closedir(d);
        return true;
      }
    }
    closedir(d);
    return false;
  } else {
    perror("opendir");
    fprintf(stderr, "Could not open '%s' in the $PATH search.\n", dirpath);
    return false;
  }
  return false;
}

int type_p_builtin(char const *exe_name) {
  // avoid mutating the return value from getenv() directly, even though we can.
  // we'll invalidate and overwrite the ACTUAL environment.
  char *env_path = getenv("PATH");
  char path[strlen(env_path) + 1];
  strcpy(path, env_path);
  char *path_base = path;
  char *next_colon;

#define TRY_PATH(path)                                                         \
  {                                                                            \
    if (is_in_dir(exe_name, path_base)) {                                      \
      printf("%s is %s/%s.\n", exe_name, path_base, exe_name);                 \
      return 0;                                                                \
    }                                                                          \
  }

  while ((next_colon = strchr(path_base, ':'))) {
    next_colon[0] = '\0';
    TRY_PATH(path_base);
    path_base = next_colon + 1;
  }
  TRY_PATH(path_base); // get the trailing path.

#undef TRY_PATH

  fprintf(stderr, "'%s' not found in PATH.\n", exe_name);
  return 1;
}

int type_builtin(int argc, char *argv[]) {
  int i = 0;
  while (++i < argc) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'p': {
        if (i + 1 >= argc) {
          fprintf(stderr, "USAGE: type -p <name_of_executable>, you didn't "
                          "provide a name.\n");
          return 1;
        } else {
          return type_p_builtin(argv[i + 1]);
        }
      } break;
      }
    }
  }

  fprintf(stderr, "Invalid arguments passed to type.\n");
  return 1;
}

#define INSERT(name_lit, fn_ptr)                                               \
  w_cm_insert(&builtin_map, name_lit,                                          \
              &(Builtin){.fn = fn_ptr, .name = name_lit "\0"})

MAKE_WCOLMAP(builtin_map, sizeof(Builtin), 1031, {
  INSERT("cd", cd_builtin);
  INSERT("exit", exit_builtin);
  INSERT("history", history_builtin);
  INSERT("export", export_builtin);
  INSERT("alias", alias_builtin);
  INSERT("type", type_builtin);
  INSERT("testargs", testargs_builtin);
});

#undef INSERT
