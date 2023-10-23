#pragma once

typedef unsigned int uint;

typedef struct WArray {
  uint full_elm_sz; // the elm sz passed in plus the sizeof the element header.
  uint elm_sz;
  uint num_elms;

  uint upper_bound; // so that we don't have to loop over the whole array and
                    // check for NULLs in each blank spot, we keep an upper
                    // bound. use this in for loop iterators.

  // the general purpose, dynamically allocated buffer for the internal array
  // data.
  void *buffer;
} WArray;

// allocate the buffer and fill the passed in WArray pointer.
void w_make_array(WArray *warray, uint elm_sz, uint num_elms);

// return a pointer to the element, or NULL if we failed to insert.
void *w_array_insert(WArray *array, void *data);
// same as insert, just only allocate and let the caller figure out the memcpy
// business.
void *w_array_alloc(WArray *array);

void w_array_delete_index(WArray *array, uint index);
void w_array_delete_ptr(WArray *array, void *ptr);

void w_array_delete_ptr(WArray *array, void *elm_ptr);

// returns NULL when something is already in the slot, and the pointer to the
// element itself (past the header) when it's fresh.
void *w_array_insert_index(WArray *array, uint index, void *data);

// just return the pointer to the slot with no checks. return NULL if there's
// already something there. consume the slot and mark it as used if it's new
// when we call this function.
void *w_array_get_slot_ptr(WArray *array, uint index);

// either return a pointer to the data in the buffer, or NULL if it doesn't
// exist.
void *w_array_get(WArray *array, uint index);

void w_clean_array(WArray *array);
