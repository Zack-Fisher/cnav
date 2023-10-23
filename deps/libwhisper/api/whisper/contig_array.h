#pragma once

#include "string.h"

/*
 * A Contiguous Array is a data structure that stores elements in a continuous
 * block of memory. This means that all elements are stored side by side in
 * memory, without any gaps or fragmentation. Each element in the array is
 * stored at a fixed memory offset from the beginning, allowing for quick and
 * direct access to individual elements.
 */

// now, get ready for some """"GENERIC PROGRAMMING""""
#define DEFINE_CONTIG_ARRAY_HEADERS(TYPE, MAX_CA_SLOTS)                        \
  TYPE##Slot w_ca_add_##TYPE(W_CA_##TYPE *contig_array, TYPE *elm);            \
  void w_ca_remove_##TYPE(W_CA_##TYPE *contig_array, TYPE##Slot slot);

// provide space for the user to padd out the struct if they'd like.
#define DEFINE_CONTIG_ARRAY_TYPES(TYPE, MAX_CA_SLOTS, padding1, padding2,      \
                                  padding3)                                    \
  typedef struct {                                                             \
    padding1 TYPE TYPE##_elms[MAX_CA_SLOTS];                                   \
    padding2 int n_##TYPE##s;                                                  \
    padding3                                                                   \
  } W_CA_##TYPE;                                                               \
                                                                               \
  typedef int TYPE##Slot;

// if the W_CA isn't zero initted before you pass it in, it'll segfault.
// allow the user to define what they'd like to happen when an error occurs by
// simply passing in a full block of code.
#define DEFINE_CONTIG_ARRAY_SOURCE(TYPE, MAX_CA_SLOTS, ERROR_BLOCK)            \
  TYPE##Slot w_ca_add_##TYPE(W_CA_##TYPE *contig_array, TYPE *elm) {           \
    TYPE##Slot this_slot = contig_array->n_##TYPE##s;                          \
    if (this_slot >= MAX_CA_SLOTS) {                                           \
      ERROR_BLOCK                                                              \
    } else {                                                                   \
      memcpy(&contig_array->TYPE##_elms[this_slot], elm, sizeof(TYPE));        \
      contig_array->n_##TYPE##s++;                                             \
    }                                                                          \
    return this_slot;                                                          \
  }                                                                            \
                                                                               \
  void w_ca_remove_##TYPE(W_CA_##TYPE *contig_array, TYPE##Slot slot) {        \
    if (slot >= MAX_CA_SLOTS) {                                                \
      ERROR_BLOCK                                                              \
    }                                                                          \
                                                                               \
    TYPE##Slot last_slot = contig_array->n_##TYPE##s - 1;                      \
    if (slot == last_slot) {                                                   \
    } else {                                                                   \
      for (int i = slot + 1; i < contig_array->n_##TYPE##s; i++) {             \
        contig_array->TYPE##_elms[i - 1] = contig_array->TYPE##_elms[i];       \
      }                                                                        \
    }                                                                          \
    contig_array->n_##TYPE##s--;                                               \
  }
