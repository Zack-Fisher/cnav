#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void dr_clean(DIR **d) { closedir(*d); }

bool find_best_completion(const char *working_path, const char *base_path,
                          char *comp_buf, int buf_len) {
  struct dirent *de;
  __attribute__((cleanup(dr_clean))) DIR *dr = opendir(base_path);
  if (dr == NULL) {
    return false;
  }

  bool found = false;
  while ((de = readdir(dr)) != NULL) {
    if (strncasecmp(working_path, de->d_name, strlen(working_path)) == 0) {
      // if it's a directory, put a slash.
      snprintf(comp_buf, buf_len, "%s%s", de->d_name,
               (de->d_type == DT_DIR) ? "/" : "");
      found = true;
      break; // Stop after finding the first match, could be improved
    }
  }
  return found;
}

bool path_completion_get(const char *curr_input, char *comp_buf, int buf_len) {
  char input_buf[256]; // Use a constant for size, replace 256 as needed
  strncpy(input_buf, curr_input, sizeof(input_buf) - 1);
  input_buf[sizeof(input_buf) - 1] = '\0';

  char *last_space = strrchr(input_buf, ' ');
  char *base_path = ".";
  char *working_path;
  char pre_buf[256] = {0}; // Again, replace 256 as needed

  if (last_space) {
    *last_space = '\0';
    last_space++;
    strncpy(pre_buf, input_buf, sizeof(pre_buf) - 1);
  } else {
    last_space = input_buf;
  }

  char *last_slash = strrchr(last_space, '/');
  if (last_slash) {
    *last_slash = '\0';
    base_path = last_space;
    working_path = last_slash + 1;
  } else {
    working_path = last_space;
  }

  char best_comp[256] = {0}; // Replace 256 as needed
  bool found = find_best_completion(working_path, base_path, best_comp,
                                    sizeof(best_comp));
  if (found) {
    snprintf(comp_buf, buf_len, "%s%s%s/%s", pre_buf,
             last_space == input_buf ? "" : " ", base_path, best_comp);
  }

  return found;
}

bool completion_get(char const *curr_input, char *comp_buf, int buf_len) {
  // how can we decide contextually what's the best type of completion to do?
  return path_completion_get(curr_input, comp_buf, buf_len);
}
