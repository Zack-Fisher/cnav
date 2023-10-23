#include "complete.h"
#include "main.h"

#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <unistd.h>

void dr_clean(DIR **d) { closedir(*d); }

static bool path_completion_get(char const *curr_input, char *comp_buf,
                                int buf_len) {
  char input_buf[MAX_INPUT_LEN];
  strncpy(input_buf, curr_input, MAX_INPUT_LEN);

  // this is making the assumption that the filepaths don't have spaces in them
  // for now.
  char *last_space = strrchr(input_buf, ' ');
  bool is_space_in_cmd = false;
  if (last_space) {
    is_space_in_cmd = true;
    // null terminate so that we can use the input_buf beginning later.
    last_space[0] = '\0';
    // must point to the first character.
    last_space++;
  } else {
    last_space =
        input_buf; // skip to the last word.
                   // for example: "cd Docum<Tab>", we need to start at 'D',
                   // not the beginning of the actual commandline 'c'.
  }

  // if there is no path, open the current directory.
  char *base_path = ".";
  char *last_slash = strrchr(last_space, '/');
  if (last_slash != NULL) {
    base_path = last_space;
    last_slash[0] = '\0';
  } else {
    // then the "last slash" is the beginning of the commandline. we still need
    // to interpret it like the end of a path.
    last_slash = last_space - 1;
  }
  // the last bit of the path that the user is still typing, this is what we'll
  // try to complete.
  char *const working_path = last_slash + 1;

  struct dirent *de;
  __attribute__((cleanup(dr_clean))) DIR *dr = opendir(base_path);
  if (dr == NULL) {
    return false;
  }

  while ((de = readdir(dr)) != NULL) {
    // if the working path is a subset of the d_name, accept this completion.
    if (strncasecmp(working_path, de->d_name, strlen(working_path)) == 0) {
      // TODO: more sophisticated, get the best match.
      snprintf(comp_buf, buf_len, "%s%s%s/%s", input_buf,
               is_space_in_cmd ? " " : "", base_path, de->d_name);
      return true;
    }
  }

  return false;
}

bool completion_get(char const *curr_input, char *comp_buf, int buf_len) {
  // how can we decide contextually what's the best type of completion to do?
  return path_completion_get(curr_input, comp_buf, buf_len);
}
