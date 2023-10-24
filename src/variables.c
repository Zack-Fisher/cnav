#include "command.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void update_variables(int new_err_code) {
  char value_buf[12];
  snprintf(value_buf, 12, "%d", new_err_code);
  setenv("?", value_buf, true);
  setenv("_", last_arg_buf, true);
}
