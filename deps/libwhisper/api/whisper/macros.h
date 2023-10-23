#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define PRINT_PTR(ptr)                                                         \
  { printf("ptr " #ptr ": %p\n", ptr); }

#define PRINT_INT(int_val)                                                     \
  { printf("int " #int_val ": %d\n", int_val); }

#define PRINT_STRING(str_val)                                                  \
  { printf("string " #str_val ": %s\n", str_val); }

#define PRINT_DOUBLE(double_val)                                               \
  { printf("double " #double_val ": %f\n", double_val); }

#define PRINT_FLOAT(float_val)                                                 \
  { printf("float " #float_val ": %f\n", float_val); }

// used for debugging, figuring out where a program segfaults by exiting.
#define STOP                                                                   \
  {                                                                            \
    printf("Force exit with STOP at: %d, %s\n", __LINE__, __FILE__);           \
    exit(0);                                                                   \
  }

#define ASSERT(stmt)                                                           \
  {                                                                            \
    printf("asserting [%s, %d, %s]: " #stmt ".\n", __PRETTY_FUNCTION__,        \
           __LINE__, __FILE__);                                                \
    assert(stmt);                                                              \
    printf("assert passed: " #stmt ".\n");                                     \
  }
