#include "whisper/array.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

// this is a struct specific to the implementation of this ADT.
typedef struct ElementHeader {
  uint is_in_use; // 1 for yes.
} ElementHeader;

void w_make_array(WArray *warray, uint elm_sz, uint num_elms) {
  // each element is one part the specified size and another part the element
  // header, indicating stuff like whether the block is in use or not.
  warray->num_elms = num_elms;
  warray->elm_sz = elm_sz;
  warray->full_elm_sz = warray->elm_sz + sizeof(ElementHeader);
  warray->upper_bound =
      0; // basically a psuedo-length for this array, even though it's not
         // properly contiguous like the other ADT in this library.

  // then, setup the actual buffer.
  uint buf_sz = warray->full_elm_sz * warray->num_elms;
  warray->buffer = malloc(buf_sz);
  // memsetting to 0 will implicitly set all the headers to "unused".
  memset(warray->buffer, 0, buf_sz);
}

void *w_array_alloc(WArray *array) {
  // first, find a proper element.
  for (int i = 0; i < array->num_elms; i++) {
    uint i_elm_offset = array->full_elm_sz * i;
    // add OFFSET bytes to the base pointer.
    uint8_t *i_elm_ptr = (uint8_t *)array->buffer + i_elm_offset;

    ElementHeader h;
    memcpy(&h, i_elm_ptr, sizeof(ElementHeader));
    if (!(h.is_in_use)) {
      // write the header in.
      // update the header we already have stack allocated, and write that
      // back into the buffer index base pointer.
      h.is_in_use = 1;
      memcpy(i_elm_ptr, &h, sizeof(ElementHeader));
      // bump past the header.
      i_elm_ptr += sizeof(ElementHeader);

      if (i == array->upper_bound) {
        // if we're inserting at the upper bound, we need to re-bump the upper
        // bound.
        array->upper_bound++;
      }

      // return the pointer to the element.
      return i_elm_ptr;
    }
  }

  return NULL;
}

// this is just a data copy. the buffer is flat, and there isn't a bunch of
// misdirection.
void *w_array_insert(WArray *array, void *data) {
  void *i_elm_ptr = w_array_alloc(array);

  if (i_elm_ptr)
    memcpy(i_elm_ptr, data, array->elm_sz);

  return i_elm_ptr;
}

// this is basically an insertion function. we just don't insert anything,
// instead giving the caller full reign over the pointer.
void *w_array_get_slot_ptr(WArray *array, uint index) {
  uint i_elm_offset =
      array->full_elm_sz * index; // add OFFSET bytes to the base pointer.
  uint8_t *i_elm_ptr = (uint8_t *)array->buffer + i_elm_offset;

  ElementHeader h;
  memcpy(&h, i_elm_ptr, sizeof(ElementHeader));
  if (!(h.is_in_use)) {
    // set it as used, we're taking this slot now.
    h.is_in_use = true;
    memcpy(i_elm_ptr, &h, sizeof(ElementHeader));
    i_elm_ptr += sizeof(ElementHeader);
    return i_elm_ptr; // return bumped pointer to the actual element.
  } else {
    return NULL;
  }
}

void w_array_delete_index(WArray *array, uint index) {
  uint index_elm_offset = array->full_elm_sz * index;
  uint8_t *index_elm_ptr = (uint8_t *)array->buffer + index_elm_offset;

  if (index == array->upper_bound - 1) {
    array->upper_bound--;
  }

  memset(index_elm_ptr, 0, sizeof(ElementHeader));
}

void w_array_delete_ptr(WArray *array, void *elm_ptr) {
  if (elm_ptr == array->buffer +
                     ((array->upper_bound - 1) * array->full_elm_sz) +
                     sizeof(ElementHeader)) {
    // deleting at the end of the array.
    array->upper_bound--;
  }

  // zero out the header, not the element itself.
  memset(elm_ptr - sizeof(ElementHeader), 0, sizeof(ElementHeader));
}

void *w_array_insert_index(WArray *array, uint index, void *data) {
  uint index_elm_offset = array->full_elm_sz * index;
  uint8_t *index_elm_ptr = (uint8_t *)array->buffer + index_elm_offset;

  ElementHeader h;
  memcpy(&h, index_elm_ptr, sizeof(ElementHeader));
  if (!(h.is_in_use)) {
    // update the header we already have stack allocated, and write that
    // back into the buffer index base pointer.
    h.is_in_use = 1;
    memcpy(index_elm_ptr, &h, sizeof(ElementHeader));
    // bump past the header.
    index_elm_ptr += sizeof(ElementHeader);

    if (index == array->upper_bound) {
      // if we're inserting at the upper bound, we need to re-bump the upper
      // bound.
      array->upper_bound++;
    }

    // memcpy the pointer into the right slot.
    memcpy(index_elm_ptr, data, array->elm_sz);

    // return the BUMPED pointer.
    return index_elm_ptr;
  } else {
    return NULL;
  }
}

void *w_array_get(WArray *array, uint index) {
  uint index_elm_offset = array->full_elm_sz * index;
  // this is the actual data's global offset, so the caller can use it.
  uint8_t *index_elm_ptr = (uint8_t *)array->buffer + index_elm_offset;

  ElementHeader h;
  memcpy(&h, index_elm_ptr, sizeof(ElementHeader));
  // don't return a pointer to the ElementHeader, the caller doesn't care about
  // that.
  index_elm_ptr += sizeof(ElementHeader); // bump past

  if (h.is_in_use) {
    return index_elm_ptr; // return the bumped pointer.
  } else {
    return NULL;
  }
}

void w_clean_array(WArray *array) {
  free(array->buffer);
  array->buffer = NULL;
  // don't free the array, we haven't necessarily alloced that on the heap.
}
