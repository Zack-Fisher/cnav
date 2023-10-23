#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
#include <stdint.h>

#define HASHMAP_SIZE 4096

typedef union WHashMapValue { // union is just a compiler-known way of "multiple
                              // interpretations of the same data".
  int as_int;
  uint64_t as_uint;
  char as_char;
  double as_double;
  void *as_ptr;
} WHashMapValue;

typedef WHashMapValue WHashMap[HASHMAP_SIZE];

/* either call this which zero-allocs it for you, or statically zero-alloc it
 yourself. we rely on pointers being NULL when they're not in use. */
void w_create_hm(WHashMap dest);
void w_free_hm(WHashMap map);

/* NOTE: all "put" methods return the index of the hashmap that the value was
 * inserted into. */

/* stick a bunch of bytes in the pointer, have the hashmap store a copy. */
unsigned int w_hm_put_ptr_clone(WHashMap map, const char *key,
                                WHashMapValue value, int value_sz);
/* store the actual value passed in. */
unsigned int w_hm_put_direct_value(WHashMap map, const char *key,
                                   WHashMapValue value);

WHashMapValue w_hm_get(WHashMap map, const char *key);

bool w_hm_delete_ptr(WHashMap map, const char *key);
bool w_hm_delete_value(WHashMap map, const char *key);

WHashMapValue w_hm_get(WHashMap map, const char *key);

/* return whether it was actually found and deleted or not. */
bool w_hm_delete(WHashMap map, const char *key);

#endif
