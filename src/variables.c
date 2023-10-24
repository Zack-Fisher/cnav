#include "variables.h"
#include "command.h"
#include <stdio.h>
#include <string.h>

#define INSERT(name_lit, value_lit)                                            \
  w_cm_insert(&variable_map, name_lit,                                         \
              &(Variable){.name = name_lit "\0", .value = value_lit "\0"})

int insert_variable(char const *name, char const *value) {
  Variable v;
  // this sucks, we shouldn't need to copy this here. bad hashmap api.
  strncpy(v.name, name, MAX_VARIABLE_NAME_LEN);
  strncpy(v.value, value, MAX_VARIABLE_VALUE_LEN);
  w_cm_insert(&variable_map, name, &v);
  return 0;
}

int change_variable(char const *name, char const *value) {
  Variable *v = w_cm_get(&variable_map, name);
  if (v) {
    strncpy(v->value, value, MAX_VARIABLE_VALUE_LEN - 1);
    return 0;
  } else {
    return 1;
  }
}

void update_variables(int new_err_code) {
  char value_buf[12];
  snprintf(value_buf, 12, "%d", new_err_code);
  change_variable("?", value_buf);
  change_variable("_", last_arg_buf);
}

MAKE_WCOLMAP(variable_map, sizeof(Variable), 509, {
  INSERT("_", "0");
  INSERT("?", "0");
});

#undef INSERT
