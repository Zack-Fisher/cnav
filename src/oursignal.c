#include "oursignal.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define _GNU_SOURCE
#include <execinfo.h>
#include <signal.h>

static void segfault_handler(int sig) {
  void *array[50];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 50);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

void install_signal_handlers() {
  signal(SIGSEGV, segfault_handler); // install handlers
}
