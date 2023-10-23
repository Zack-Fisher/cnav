#include "../src/complete.h"
#include "../src/main.h"

#include <stdio.h>

#define TEST_PATH_COMPLETION(test_lit)                                         \
  {                                                                            \
    char comp_buf[MAX_INPUT_LEN];                                              \
    bool res = path_completion_get(test_lit, comp_buf, MAX_INPUT_LEN);         \
    if (res) {                                                                 \
      printf("Completed path: '%s' -> '%s'\n", test_lit, comp_buf);            \
    } else {                                                                   \
      printf("No completion found: '%s'.\n", test_lit);                        \
    }                                                                          \
  }

int main() {
  TEST_PATH_COMPLETION("Make");
  TEST_PATH_COMPLETION("MAKE");
  TEST_PATH_COMPLETION("");
  TEST_PATH_COMPLETION("cd");
  TEST_PATH_COMPLETION("cd ");
  TEST_PATH_COMPLETION("cd makef");
  TEST_PATH_COMPLETION("cd Makef");
  TEST_PATH_COMPLETION("cd te");
  TEST_PATH_COMPLETION("cd test_");
  return 0;
}
