#include "whisper/colmap.h"
#include "whisper/macros.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_cm() {
  WColMap map;

  // Test 1: Creating a ColMap with element size sizeof(int) and initial map
  // size 10
  w_create_cm(&map, sizeof(int), 9000);
  assert(map.elm_sz == sizeof(int));
  assert(map.num_elms == 9000); // assuming num_elms stores the initial map size

  // Test 2: Inserting key-value pairs
  int test_values[3] = {1, 2, 3};
  w_cm_insert(&map, "first", &test_values[0]);
  w_cm_insert(&map, "second", &test_values[1]);
  w_cm_insert(&map, "third", &test_values[2]);
  w_cm_insert(&map, "thirdd", &test_values[2]);
  w_cm_insert(&map, "thirddd", &test_values[2]);
  w_cm_return_slot(&map, "thirdddd");
  w_cm_return_slot(&map, "thirddddd");
  w_cm_return_slot(&map, "thirdddddd");
  w_cm_return_slot(&map, "thirddddddd");

  int *g = w_cm_return_slot(&map, "gouraud");
  int *p_g = w_cm_return_slot(&map, "pbr_gouraud");
  ASSERT(g);
  ASSERT(p_g);

  // Test 3: Getting values by key
  int *value = (int *)w_cm_get(&map, "first");
  assert(*value == 1);

  value = (int *)w_cm_get(&map, "second");
  assert(*value == 2);

  value = (int *)w_cm_get(&map, "third");
  assert(*value == 3);

  // Test 4: Deleting a key-value pair
  w_cm_delete(&map, "second");
  value = (int *)w_cm_get(&map, "second");
  assert(value == NULL);

  // Test 5: Cleaning up
  w_free_cm(&map);
  // Add any cleanup validation if necessary
}
