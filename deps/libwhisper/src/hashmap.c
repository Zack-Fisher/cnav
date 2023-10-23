#include "whisper/hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int w_hm_hash(const char *key) {
  unsigned int hash = 0;
  while (*key) {
    // const char * makes since, *key++ is modifying the copied pointer THEN
    // dereffing.
    hash = (hash << 5) + *key++;
  }
  // bound the key in the actual hashing function.
  return hash % HASHMAP_SIZE;
}

// pass in an array ptr of size (HASHMAP_SIZE * WHashMapValue).
void w_create_hm(WHashMap dest) {
  memset(dest, 0, sizeof(WHashMapValue) * HASHMAP_SIZE);
}

void w_free_hm(WHashMap map) {
  for (int i = 0; i < HASHMAP_SIZE; i++) {
    WHashMapValue current = map[i];
    // TODO:
    // i actually have no idea how to do this. does there need to be seperate
    // state in the map keeping track of which elements are pointers? we can't
    // just free the random values.
    //
    // if (current) {
    //   free(current);
    // }
  }
}

unsigned int w_hm_put_ptr_clone(WHashMap map, const char *key,
                                WHashMapValue value, int value_sz) {
  unsigned int index = w_hm_hash(key);
  WHashMapValue new_value = (WHashMapValue){.as_ptr = malloc(value_sz)};
  memcpy(new_value.as_ptr, value.as_ptr, value_sz);

  map[index] = new_value;
  return index;
}

unsigned int w_hm_put_direct_value(WHashMap map, const char *key,
                                   WHashMapValue value) {
  unsigned int index = w_hm_hash(key);

  map[index] = value;
  return index;
}

WHashMapValue w_hm_get(WHashMap map, const char *key) {
  unsigned int index = w_hm_hash(key);
  WHashMapValue current = map[index];
  return current;
}

bool w_hm_delete_ptr(WHashMap map, const char *key) {
  unsigned int index = w_hm_hash(key);
  WHashMapValue current = map[index];
  if (current.as_ptr) { // if the value isn't all zero bits.
    free(current.as_ptr);
    (map[index]).as_ptr = NULL;
    return true;
  }
  return false;
}

bool w_hm_delete_value(WHashMap map, const char *key) {
  unsigned int index = w_hm_hash(key);
  WHashMapValue current = map[index];
  // we still consider a zeroed out value to be blank in the hashtable. we just
  // don't free() the value in this call.
  if (current.as_ptr) { // if the value is all zero bits.
    (map[index]).as_ptr = NULL;
    return true;
  }
  return false;
}
