#include "../src/builtin.h"
#include "../src/command.h"
#include "../src/complete.h"
#include "../src/main.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

void test_expansion(char const *input) {
  int input_len = strlen(input);
  int expansion_len = input_len * 4;
  char expand_buf[expansion_len];
  cmd_expand(input, input_len, expand_buf, expansion_len);
  printf("Shell expansion: '%s' -> '%s'\n", input, expand_buf);
}

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
  printf("testing\n");

  TEST_PATH_COMPLETION("Make");
  TEST_PATH_COMPLETION("MAKE");
  TEST_PATH_COMPLETION("");
  TEST_PATH_COMPLETION("cd");
  TEST_PATH_COMPLETION("cd ");
  TEST_PATH_COMPLETION("cd makef");
  TEST_PATH_COMPLETION("cd Makef");
  TEST_PATH_COMPLETION("cd te");
  TEST_PATH_COMPLETION("cd test_");

  test_expansion("~/lkasjdf");
  test_expansion("ls grep man");
  test_expansion("ls");
  test_expansion("ls");
  test_expansion("ls");
  test_expansion("ls ls");
  test_expansion("grep | blah");
  test_expansion("grep | blah");
  test_expansion("grep | blah");
  test_expansion("grep | blah");
  test_expansion("~");

  printf("done testing\n");

  return 0;
}
