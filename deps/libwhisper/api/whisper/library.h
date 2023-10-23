#pragma once

// a cross-platform dll/so/dylib system.

#include "whisper/colmap.h"

// use a proc table for caching.
#define COMMON_FIELDS WColMap proc_table

#ifdef __unix__
#define LIBRARY_PREFIX "lib"
#define LIBRARY_SUFFIX ".so"

typedef struct Lib {
  COMMON_FIELDS;
  void *handle;
} Lib;

#elif defined(_WIN32)
#define LIBRARY_PREFIX ""
#define LIBRARY_SUFFIX ".dll"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "whisper/colmap.h"

typedef struct Lib {
  COMMON_FIELDS;
HMODULE hModule; // windows handle to a dll.
} Lib;

#else
#error "Unsupported platform"
#endif

// Lib* structures should be re-usable easily. this is what the api's designed
// around.

// just make the generic object, whatever's around the platform.
void make_lib(Lib *lib);
// actually load a library context into the object.
bool load_lib(Lib *lib, const char *path);
void *lib_get_sym(Lib *lib, const char *name);
void lib_free(Lib *lib);
