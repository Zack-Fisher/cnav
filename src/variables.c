#include "variables.h"
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

MAKE_WCOLMAP(variable_map, sizeof(Variable), 509, {
  INSERT("_", "0");
  INSERT("?", "0");
});

#undef INSERT
