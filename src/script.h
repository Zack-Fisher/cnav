#pragma once

// define a headless mode that allows us to run commands directly from scripts.

// pass NULL to fallback on the default _shell mode.
int setup_mode(char const *init_mode);

int run_script(const char *filepath);
int handle_input_line(char *input);
