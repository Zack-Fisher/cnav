#pragma once

// implementation of a gap buffer.
typedef struct {
  // the usual stuff
  void *buffer;
  int elm_sz;
  int num_elms;

  // the cursor is thought of as positioned directly to the left of the gap, if
  // you're thinking of this as a line in a text editor.
  // these are both INDICES.
  int gap_start;
  int gap_end;
} WGapBuf;

// pass this the first value that goes at the cursor at init-time.
void w_gb_create(WGapBuf *l, int elm_sz, int num_elms, void *init_value);
// pass in the base pointer of a whole block of initialization values, all of
// which will be inserted.
void w_gb_create_from_block(WGapBuf *l, int elm_sz, int num_elms,
                            void *init_values_base, int num_init_values);

void w_gb_free(WGapBuf *l);

int w_gb_get_length(WGapBuf *l);

// get the pointer to the value at the cursor.
void *w_gb_read(WGapBuf *l);

int w_gb_shift_left(WGapBuf *l);
int w_gb_shift_right(WGapBuf *l);

void w_gb_go_to_beginning(WGapBuf *l);
void w_gb_go_to_end(WGapBuf *l);

int w_gb_get_length_before_gap(WGapBuf *l);
int w_gb_get_length_after_gap(WGapBuf *l);

void w_gb_shift_by(WGapBuf *l, int by);

void w_gb_shift_to(WGapBuf *l, int index);

// copy the value into the slot.
void w_gb_insert(WGapBuf *l, void *value);
// just return the slot.
void *w_gb_insert_grab(WGapBuf *l);

// delete at gap/cursor implicitly.
int w_gb_delete(WGapBuf *l);

// everything after the cursor is deleted, so basically removing every element
// after the gap.
void w_gb_delete_after_cursor(WGapBuf *l);

void w_gb_debug_print(WGapBuf *l, int up_to);
