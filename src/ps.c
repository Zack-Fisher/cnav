#include "ps.h"
#include "ansi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// NOTE: ps = "prompt string", eg the PS1, PS2, ... variables in bash.
typedef struct PSState {
  char buf[MAX_PS_LEN];
  char modes[MAX_MODES][MAX_MODE_LEN];
  char username[MAX_USERNAME_LEN];
  char hostname[MAX_HOSTNAME_LEN];
  int err_code;
} PSState;

static PSState _ps = {0};

static void _sync_ps();

__attribute__((constructor)) void init_ps() {
  char *username = getenv("USER");
  strncpy(_ps.username, username, MAX_USERNAME_LEN);
  gethostname(_ps.hostname, MAX_HOSTNAME_LEN);
  _sync_ps();
}

// HUP/rerender the prompt string whenever the state is internally changed.
static void _sync_ps() {
  memset(_ps.buf, 0, MAX_PS_LEN); // might as well.
  char *_ptr = _ps.buf;
  if (_ps.err_code == 0) {
    _ptr += sprintf(_ptr, "[ " ANSI_GREEN "%d " ANSI_RESET, _ps.err_code);
  } else {
    _ptr += sprintf(_ptr, "[ " ANSI_RED "%d " ANSI_RESET, _ps.err_code);
  }

  int num_modes = 0;
  for (int i = 0; i < MAX_MODES; i++) {
    if (_ps.modes[i][0] != '\0') {
      if (num_modes == 0) {
        // if this is the first mode, have the opening paren.
        _ptr += sprintf(_ptr, "(");
      }
      _ptr += sprintf(_ptr, " %s,", _ps.modes[i]);
      num_modes++;
    }
  }

  if (num_modes > 0) { // strip the comma and replace with the closing ).
    _ptr--;
    _ptr += sprintf(_ptr, " ) ");
  }

  _ptr +=
      sprintf(_ptr, ANSI_GREEN "%s" ANSI_YELLOW "@" ANSI_GREEN "%s" ANSI_RESET,
              _ps.username, _ps.hostname);

  char cwd[512];
  if (getcwd(cwd, 512)) {
    _ptr += sprintf(_ptr, ANSI_BLUE " %s" ANSI_RESET, cwd);
  } else {
    perror("getcwd");
  }

  _ptr += sprintf(_ptr, " ]> ");
}

void add_ps_mode(char const *mode) {
  for (int i = 0; i < MAX_MODES; i++) {
    if (_ps.modes[i][0] == '\0') {
      strncpy(_ps.modes[i], mode, MAX_MODE_LEN);
      _sync_ps();
      return;
    }
  }

  fprintf(stderr, "Too many modes, can't add '%s'.\n", mode);
}

void remove_ps_mode(char const *mode) {
  for (int i = 0; i < MAX_MODES; i++) {
    // printf("strncmp(%s, %s, %d);\n", _ps.modes[i], mode, MAX_MODE_LEN);
    if (strncmp(_ps.modes[i], mode, MAX_MODE_LEN) == 0) {
      memset(_ps.modes[i], 0, MAX_MODE_LEN);
      _sync_ps();
      return;
    }
  }

  fprintf(stderr, "Didn't find '%s' in the mode list.\n", mode);
}

void set_ps_error_code(int code) {
  _ps.err_code = code;
  _sync_ps();
}

const char *get_ps() { return _ps.buf; }
