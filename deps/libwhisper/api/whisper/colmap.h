#pragma once

/*
 * very similar interface to a hashmap, but the implementation is using a warray
 * so it has basic/fast collision detection.
 * */

#include "whisper/array.h"
#include <stdbool.h>

typedef unsigned int uint;

typedef WArray WColMap;

void w_create_cm(WColMap *dest, int elm_sz, int map_sz);

void w_free_cm(WColMap *map);

void *w_cm_insert(WColMap *map, const char *key, void *value);

// just return the slot without inserting. return NULL if there's already
// something there. this marks the slot as "used" in the header.
void *w_cm_return_slot(WColMap *map, const char *key);

void *w_cm_get(WColMap *map, const char *key);

void w_cm_delete(WColMap *map, const char *key);

#define MAKE_WCOLMAP(name, elm_sz, map_sz, ...) 					\
	WColMap name;									\
	void make_colmap_##name() __attribute__((constructor)); 			\
	void make_colmap_##name() {							\
		w_create_cm(&name, elm_sz, map_sz);					\
		__VA_ARGS__ 								\
	}										\
	void clean_colmap_##name() __attribute__((destructor)); 			\
	void clean_colmap_##name() {							\
		w_free_cm(&name);							\
	}
