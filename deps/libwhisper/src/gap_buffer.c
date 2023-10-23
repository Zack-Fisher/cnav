#include "whisper/gap_buffer.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, min_val, max_val) MIN(max_val, MAX(x, min_val))

// be sure to keep the gap NULLed for easy printf'ing.

void w_gb_create(WGapBuf *l, int elm_sz, int num_elms, void *init_value) {
  l->elm_sz = elm_sz;
  l->num_elms = num_elms;
  l->buffer = calloc(elm_sz, num_elms);
  memcpy(&l->buffer[0], init_value, l->elm_sz);
  l->gap_start = 1; // starts at the first index.
  l->gap_end =
      l->num_elms - 1; // the end of the gap is the last index in the buffer.
}

void w_gb_create_from_block(WGapBuf *l, int elm_sz, int num_elms,
                            void *init_values_base, int num_init_values) {
  assert((num_init_values < num_elms) &&
         "Your initialization block is too big, either init with "
         "less values or increase the size of the gap buffer.");
  assert((num_init_values > 0) &&
         "You must init the gap buffer with at least one value.");

  l->elm_sz = elm_sz;
  l->num_elms = num_elms;
  l->buffer = calloc(elm_sz, num_elms);
  // all pushed into the left of the gap, so the gap_end doesn't move at all.
  // the cursor should point to the end of the string. call the shift to
  // beginning function if you want the cursor at the beginning of the line.
  memcpy(&l->buffer[0], init_values_base, num_init_values * elm_sz);
  l->gap_start = num_init_values; // starts at the first index.
  l->gap_end =
      l->num_elms - 1; // the end of the gap is the last index in the buffer.
}

void w_gb_free(WGapBuf *l) { free(l->buffer); }

int w_gb_get_length(WGapBuf *l) {
  return (l->gap_start + (l->num_elms - (l->gap_end + 1)));
}

void *w_gb_read(WGapBuf *l) { return &l->buffer[l->gap_start - 1]; }

// move a character from the left buffer into the right buffer.
int w_gb_shift_left(WGapBuf *l) {
  if (l->gap_start <= 1) {
    return 1;
  }

  l->gap_start--;
  memcpy(&l->buffer[l->gap_end], &l->buffer[l->gap_start], l->elm_sz);
  memset(&l->buffer[l->gap_start], 0, l->elm_sz);
  l->gap_end--;

  return 0;
}

// move a character from the right buffer into the left buffer.
int w_gb_shift_right(WGapBuf *l) {
  if (l->gap_end >= l->num_elms) {
    return 1;
  }

  l->gap_end++;
  memcpy(&l->buffer[l->gap_start], &l->buffer[l->gap_end], l->elm_sz);
  memset(&l->buffer[l->gap_end], 0, l->elm_sz);
  l->gap_start++;

  return 0;
}

void w_gb_go_to_beginning(WGapBuf *l) {
  while (w_gb_shift_left(l) == 0) {
    // wait until it returns a nonzero status to indicate error shifting left.
  }
}

void w_gb_go_to_end(WGapBuf *l) {
  while (w_gb_shift_right(l) == 0) {
  }
}

void w_gb_shift_by(WGapBuf *l, int by) {
  if (by < 0) {
    by = CLAMP(by, -l->gap_start, 0);
    for (int i = 0; i < -1 * by; i++) {
      w_gb_shift_left(l);
    }
  } else {
    by = CLAMP(by, 0, w_gb_get_length(l) - l->gap_start);
    for (int i = 0; i < by; i++) {
      w_gb_shift_right(l);
    }
  }
}

void w_gb_shift_to(WGapBuf *l, int index) {
  int curr = l->gap_start - 1;
  index = CLAMP(index, 0, w_gb_get_length(l));
  int by = (index - curr);
  printf("moving by: %d\n", by);
  w_gb_shift_by(l, by);
}

void w_gb_insert(WGapBuf *l, void *value) {
  if (l->gap_start == l->gap_end)
    return;

  // copy the cursor up
  memcpy(&l->buffer[l->gap_start], &l->buffer[l->gap_start - 1], l->elm_sz);
  memcpy(&l->buffer[l->gap_start - 1], value, l->elm_sz);
  l->gap_start++;
}

void *w_gb_insert_grab(WGapBuf *l) {
  if (l->gap_start == l->gap_end)
    return NULL;

  void *pos = &l->buffer[l->gap_start];
  l->gap_start++;
  return pos;
}

int w_gb_delete(WGapBuf *l) {
  if (l->gap_start == 1) {
    // steal one from the right buffer.
    if (l->gap_end == l->num_elms - 1) {
      // we cannot remove another without nullifying the cursor.
      return -1;
    }
    memcpy(&l->buffer[l->gap_start - 1], &l->buffer[l->gap_end + 1], l->elm_sz);
    l->gap_end++;
  } else {
    // just decrement the lower buffer.
    l->gap_start--;
    memset(&l->buffer[l->gap_start], 0, l->elm_sz);
  }

  return 0;
}

int w_gb_get_length_before_gap(WGapBuf *l) { return l->gap_start; }

int w_gb_get_length_after_gap(WGapBuf *l) {
  return (l->num_elms - (l->gap_end + 1));
}

void w_gb_delete_after_cursor(WGapBuf *l) {
  if (l->gap_start == l->num_elms - 1) {
    // we cannot remove another without nullifying the cursor.
    return;
  }

  int elms_left = w_gb_get_length_after_gap(l) - 1; // minus the end value.

  l->gap_end += elms_left;
  memset(&l->buffer[l->gap_end + 1], 0, l->elm_sz * elms_left);
}

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define PURPLE "\033[35m"

#define RESET "\033[0m"

#define VALUE_COLOR BLUE
#define GAP_COLOR RED

void w_gb_debug_print(WGapBuf *l, int up_to) {
  printf("\n");
  int max = MIN(up_to, l->num_elms);

  printf("Start WGapBuf print -> " VALUE_COLOR "| ");
  for (int i = 0; i < max; i++) {
    if (i == l->gap_start)
      printf(GAP_COLOR);
    if (i == l->gap_end + 1)
      printf(VALUE_COLOR);
    printf("%2d: ", i);
    for (int j = 0; j < l->elm_sz; j++) {

      uint8_t b = ((uint8_t *)l->buffer)[(i * l->elm_sz) + j];
      printf("0x%02X ", b);
    }
    printf("| ");
  }
  printf(RESET "<- End. \n");
}
