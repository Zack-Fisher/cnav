#include "whisper/colmap.h"
#include "whisper/library.h"
#include <stdbool.h>

inline static void _load_lib_common(Lib *lib) {
  w_create_cm(&lib->proc_table, sizeof(void *), 509);
}

// common functionality, just short-circuit if already loaded
#define CHECK_CACHE()                                                          \
  void *ptr = NULL;                                                            \
  if ((ptr = w_cm_get(&lib->proc_table, name))) {                              \
    return ptr;                                                                \
  }

#define INSERT_CACHE() w_cm_insert(&lib->proc_table, name, &ptr)

inline static void _free_lib_common(Lib *lib) { w_free_cm(&lib->proc_table); }

#ifdef __unix__
#include <dlfcn.h>
#include <stdio.h>

void make_lib(Lib *lib) {
  lib->handle = NULL;
  _load_lib_common(lib);
}

bool load_lib(Lib *lib, const char *path) {
  // close the library if we have one open, clear the OS cache.
  if (lib->handle)
    dlclose(lib->handle);

  // Load the shared library
  lib->handle = dlopen(path, RTLD_LAZY);
  if (!lib->handle) {
    return false;
  }

  // Clear any existing errors
  dlerror();

  _load_lib_common(lib);
  return true;
}

void *lib_get_sym(Lib *lib, const char *name) {
  CHECK_CACHE();

  // Load the "hello" function from the library
  ptr = dlsym(lib->handle, name);
  const char *dlsym_error = dlerror();
  if (dlsym_error) {
    return NULL;
  }

  INSERT_CACHE();
  return ptr;
}

void lib_free(Lib *lib) {
  if (lib->handle)
    dlclose(lib->handle);
  _free_lib_common(lib);
}

#elif defined(_WIN32)

void make_lib(Lib *lib) {
  lib->hModule = NULL;
  _load_lib_common(lib);
}

bool load_lib(Lib *lib, const char *path) {
  // close the library if we have one open, clear the OS cache.
  if (lib->hModule)
    FreeLibrary(lib->hModule);

  // Load the shared library
  lib->hModule = LoadLibrary(path);
  if (!lib->hModule) {
    return false;
  }

  _load_lib_common(lib);
  return true;
}

void *lib_get_sym(Lib *lib, const char *name) {
  CHECK_CACHE();

  // Load the "hello" function from the library
  ptr = GetProcAddress(lib->hModule, name);
  if (!ptr) {
    return NULL;
  }

  INSERT_CACHE();
  return ptr;
}

void lib_free(Lib *lib) {
  if (lib->hModule)
    FreeLibrary(lib->hModule);
  _free_lib_common(lib);
}
#else
#error "Unsupported platform"
#endif
