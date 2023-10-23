#pragma once

#include <stdbool.h>

bool find_best_completion(const char *working_path, const char *base_path,
                          char *comp_buf, int buf_len);
bool path_completion_get(const char *curr_input, char *comp_buf, int buf_len);

// this just fills the buffer with a null-terminated string.
// returns false if the completion doesn't fill/decide anything.
bool completion_get(char const *curr_input, char *comp_buf, int buf_len);
