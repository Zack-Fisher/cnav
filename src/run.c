#include "run.h"
#include "command.h"
#include "main.h"
#include "mode.h"
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#define MAX_FILE_LEN (2048)

static int _input_files[MODE_COUNT];

// insert the proper sentinel at each location.
__attribute__((constructor)) void init_input_files() {
  for (int i = 0; i < MODE_COUNT; i++) {
    _input_files[i] = -1;
  }
}

__attribute__((destructor)) void clean_input_files() {
  for (int i = 0; i < MODE_COUNT; i++) {
    int input_file = _input_files[i];
    if (input_file != -1) {
      close(input_file);
      if (input_file == -1) {
        perror("close(): could not close runner input file");
      }
    }
  }
}

// setup the input file, lazy init if necessary.
static int fill_file(char const *fname, char const *input_fmt,
                     char const *input_ptr, int *input_fd) {
  if (*input_fd == -1) {
    *input_fd = open(fname, O_CREAT | O_RDWR, 0644);
    if (*input_fd == -1) {
      perror("open(): could not open compiler input file");
    }
  }

  char input_file_buf[MAX_FILE_LEN];
  int len = snprintf(input_file_buf, MAX_FILE_LEN, input_fmt, input_ptr);
  // seek to the beginning of the fd.
  off_t offset = lseek(*input_fd, 0, SEEK_SET);
  if (offset == -1) {
    perror("lseek");
    return INTERNAL_ERROR;
  }

  // write the string to be compiled and run.
  write(*input_fd, input_file_buf, len);

  // then chop off the other garbage in the file.
  if (ftruncate(*input_fd, len) == -1) {
    perror("ftruncate");
    return INTERNAL_ERROR;
  }

  return 0;
}

// NOTE: this modifies the string in-place.
void input_expand_newlines(char *input_ptr) {
  int len = strlen(input_ptr);
  for (int i = 0; i < len; i++) {
    char ch = input_ptr[i];
    switch (ch) {
    case '\\': {
      i++;
      ch = input_ptr[i];
      if (ch == 'n') {
        // replace "\n" with (' ', '0x10')
        // we can do this in place since the escape code is always larger than
        // our replacement, 2 bytes -> 1 byte.
        input_ptr[i - 1] = ' ';
        input_ptr[i] = '\n';
      }
    } break;
    }
  }
}

int mode_run(Mode mode, char *input_ptr) {
  switch (mode) {

#define TRY_EXEC(...)                                                          \
  {                                                                            \
    char *argv[] = {__VA_ARGS__, NULL};                                        \
    EXECUTE_AND_COUNT(argv);                                                   \
    if (execute_and_count_result != 0) {                                       \
      return execute_and_count_result;                                         \
    }                                                                          \
  }

#define RUN_BIN(binpath)                                                       \
  {                                                                            \
    char *argv[] = {binpath, NULL};                                            \
    EXECUTE_AND_COUNT(argv);                                                   \
    return execute_and_count_result;                                           \
  }

  case C_MODE: {
    fill_file("/tmp/cnav_in.c",
              "#include <stdio.h>\n"
              "#include <stdlib.h>\n"
              "int main() {%s}",
              input_ptr, &_input_files[C_MODE]);
    // compile the string.
    TRY_EXEC("gcc", "-Wno-implicit-function-declaration", "/tmp/cnav_in.c",
             "-o", "/tmp/cnav_bin");
    RUN_BIN("/tmp/cnav_bin");
  } break;

  case NASM_MODE: {
    input_expand_newlines(input_ptr);

    fill_file("/tmp/cnav_in.asm",
              "global _start:\n"
              "section .text\n"
              "_start:\n"
              "%s\n"
              "mov rax, 60\n"
              "mov rdi, 0\n"
              "syscall\n",
              input_ptr, &_input_files[NASM_MODE]);
    TRY_EXEC("nasm", "-f", "elf64", "-o", "/tmp/cnav_object.o",
             "/tmp/cnav_in.asm");
    TRY_EXEC("ld", "-o", "/tmp/cnav_bin", "/tmp/cnav_object.o");
    RUN_BIN("/tmp/cnav_bin");
  } break;

#undef RUN_BIN
#undef TRY_EXEC

  default: {
    printf("ERROR: not an external mode, cannot compile/run.\n");
    return INTERNAL_ERROR;
  } break;
  }
}
