#pragma once

#define SHELL_NAME "cnav"

#define MAX_USERNAME_LEN (64)
#define MAX_HOSTNAME_LEN (64)
#define MAX_MODES (5)
#define MAX_MODE_LEN (16)
#define MAX_PS_LEN                                                             \
  ((MAX_MODES * MAX_MODE_LEN) + MAX_USERNAME_LEN + MAX_HOSTNAME_LEN + 64)

void add_ps_mode(char const *mode);
void remove_ps_mode(char const *mode);
void set_ps_error_code(int code);
const char *get_ps();
