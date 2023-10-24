#include "../src/builtin.h"
#include "../src/command.h"
#include "../src/complete.h"
#include "../src/main.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

void test_expand_and_tokenize(char const *line, char const *expected[]) {
  int line_len = strlen(line);
  int expansion_len = 10 * 1024;
  char expand_buf[expansion_len];
  cmd_expand(line, line_len, expand_buf, expansion_len);
  printf("Shell expansion: '%s' -> '%s'\n", line, expand_buf);
  printf("Tokenization from '%s' into:\n", expand_buf);
  int argc = tokenize_line(expand_buf);
  for (int i = 0; i < argc; i++) {
    printf("[%d]: %s\n", i, tokenize_line_result[i]);
    assert(!expected || strcmp(expected[i], tokenize_line_result[i]) == 0);
  }
}

void test_expansion(char const *input, char const *expected) {
  int input_len = strlen(input);
  int expansion_len = 10 * 1024;
  char expand_buf[expansion_len];
  cmd_expand(input, input_len, expand_buf, expansion_len);
  printf("Shell expansion: '%s' -> '%s'\n", input, expand_buf);
  assert(!expected || strcmp(expected, expand_buf) == 0);
}

void test_tokenize_line(char const *line, char const *expected[]) {
  printf("Tokenization from '%s' into:\n", line);
  int argc = tokenize_line(line);
  for (int i = 0; i < argc; i++) {
    printf("[%d]: %s\n", i, tokenize_line_result[i]);
    assert(!expected || strcmp(expected[i], tokenize_line_result[i]) == 0);
  }
}

void test_path_completion(char const *line, char const *expected) {
  char comp_buf[MAX_INPUT_LEN];
  bool res = path_completion_get(line, comp_buf, MAX_INPUT_LEN);
  if (res) {
    printf("Completed path: '%s' -> '%s'\n", line, comp_buf);
    assert(!expected || strcmp(expected, comp_buf) == 0);
  } else {
    printf("No completion found: '%s'.\n", line);
    assert(!expected);
  }
}

int main() {
  printf("testing\n");

  test_path_completion("Make", "./Makefile");
  test_path_completion("MAKE", "./Makefile");
  test_path_completion("", NULL);
  test_path_completion("cd", NULL);
  test_path_completion("cd ", NULL);
  test_path_completion("cd makef", "cd ./Makefile");
  test_path_completion("cd Makef", "cd ./Makefile");
  test_path_completion("cd te", NULL);
  test_path_completion("cd test_", NULL);
  test_path_completion("./test/Progr", "\"./test/Program Files/\"");
  test_path_completion("cd ./test/Progr", "cd \"./test/Program Files/\"");

  // character expansion
  test_expansion("~", NULL);
  test_expansion("~/lkasjdf", NULL);

  // aliasing expansion
  test_expansion("ls grep man", NULL);
  test_expansion("ls", NULL);
  test_expansion("ls", NULL);
  test_expansion("ls", NULL);
  test_expansion("ls ls", NULL);
  test_expansion("grep | blah", NULL);
  test_expansion("grep | blah", NULL);
  test_expansion("grep | blah", NULL);
  test_expansion("grep | blah", NULL);

  // variable expansion
  test_expansion("echo $?", "echo 0");

  // globbing
  test_expansion("*", NULL);
  test_expansion("ls *", NULL);

  // cancelling metacharacters
  test_expansion("\\*", "*");
  test_expansion("\\*\\!\\~\\\\", "*!~\\");
  // should do nothing. \\ doesn't cancel ' '?
  test_expansion("cd ./Program\\ Files", "cd ./Program\\ Files");

  test_tokenize_line("ls -liah", NULL);
  test_tokenize_line("blah blah stuff", NULL);
  test_tokenize_line("cd ./Program\\ Files",
                     (const char *[]){"cd", "./Program Files"});
  test_expand_and_tokenize("\'all one word\'",
                           (const char *[]){"all one word"});

  printf("done testing\n");

  return 0;
}
