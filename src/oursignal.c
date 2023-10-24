#include "oursignal.h"
#include "command.h"
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

static void passthrough_handler(int sig) {
  if (child_pid != -1) {
    kill(child_pid, sig);
  } else {
    signal(sig, SIG_DFL); // Reset to default handler
    raise(sig);           // re-raise the signal for the default handler.
    signal(sig, passthrough_handler);
  }
}

void install_signal_handlers() {
  signal(SIGSEGV, segfault_handler);

  signal(SIGINT, passthrough_handler);
  signal(SIGTSTP, passthrough_handler);
  signal(SIGQUIT, passthrough_handler);
  signal(SIGTERM, passthrough_handler);
  signal(SIGHUP, passthrough_handler);
  signal(SIGCONT, passthrough_handler);
}
