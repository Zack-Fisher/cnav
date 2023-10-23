#pragma once

#include <stdbool.h>

// this just fills the buffer with a null-terminated string.
// returns false if the completion doesn't fill/decide anything.
bool completion_get(char const *curr_input, char *comp_buf, int buf_len);
