#include "whisper/gap_buffer.h" // Make sure to include your gap buffer header file
#include <stdio.h>
#include <stdlib.h>

#include "whisper/macros.h"

#define TEST_ASSERT(message, test)                                             \
  do {                                                                         \
    if (!(test))                                                               \
      return message;                                                          \
  } while (0)

static void test_init(WGapBuf *gb) {
  int x = 4;
  w_gb_create(gb, sizeof(int), 10, &x);
  w_gb_insert(gb, &x);
  w_gb_insert(gb, &x);
  w_gb_insert(gb, &x);
  w_gb_insert(gb, &x);
  w_gb_insert(gb, &x);
}

void test_shift_left_right() {
  WGapBuf gb;
  test_init(&gb);

  printf("%d\n", w_gb_get_length(&gb));
  ASSERT(w_gb_get_length(&gb) == 6);

  w_gb_debug_print(&gb, 10);

  w_gb_shift_left(&gb);
  w_gb_shift_left(&gb);
  w_gb_debug_print(&gb, 10);
  ASSERT(gb.gap_start == 4);

  w_gb_shift_right(&gb);
  ASSERT(gb.gap_start == 5);

  w_gb_delete(&gb);
  ASSERT(gb.gap_start == 4);
  w_gb_delete(&gb);
  ASSERT(gb.gap_start == 3);
  w_gb_delete(&gb);
  ASSERT(gb.gap_start == 2);
  w_gb_delete(&gb);
  ASSERT(gb.gap_start == 1);
  w_gb_delete(&gb);
  ASSERT(gb.gap_start == 1);
  w_gb_delete(&gb);
  ASSERT(gb.gap_start == 1);

  int x = 7;
  w_gb_insert(&gb, &x);
  w_gb_insert(&gb, &x);
  w_gb_insert(&gb, &x);
  w_gb_insert(&gb, &x);
  w_gb_insert(&gb, &x);
  ASSERT(gb.gap_start == 6);

  w_gb_debug_print(&gb, 10);
  w_gb_shift_to(&gb, 3);
  w_gb_debug_print(&gb, 10);
  ASSERT(gb.gap_start == 4);

  w_gb_shift_to(&gb, 0);
  ASSERT(gb.gap_start == 1);
  w_gb_shift_to(&gb, w_gb_get_length(&gb) - 1);
  ASSERT(gb.gap_start == w_gb_get_length(&gb));

  w_gb_shift_to(&gb, 3);
  w_gb_debug_print(&gb, 10);
  w_gb_delete_after_cursor(&gb);
  w_gb_debug_print(&gb, 10);

  w_gb_free(&gb);
}

void test_gb() { test_shift_left_right(); }
