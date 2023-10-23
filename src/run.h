#pragma once

#include "mode.h"

#define CEXPR_START_STRING "__"

// handle the compilation and running of the C code after the delim in this
// module.

int mode_run(Mode mode, char *input_ptr);
