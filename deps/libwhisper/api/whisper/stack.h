#pragma once

#include <stdbool.h>

// simple stack that pushes up and pops down.

typedef unsigned int uint;

typedef struct WStack {
  uint elm_sz;
  uint num_elms;

  void *stack_pointer; // the current location

  void *base_pointer; // the base of the stack data buffer.

  void *top_pointer; // the top of the stack
} WStack;

void w_stack_create(WStack *stack, uint elm_sz, uint num_elms);

void *w_stack_get_curr(WStack *stack);
bool w_stack_at_bottom(WStack *stack);

// return either the value or NULL if we're at the bottom of the stack.
void *w_stack_pop(WStack *stack);

// return the space for you to write your data or NULL if there's none left.
void *w_stack_push(WStack *stack);

// basically just stack_pointer = base_pointer. reset the internal state to the
// bottom of the stack.
void w_stack_pop_all(WStack *stack);

void w_stack_clean(WStack *stack);

void w_stack_debug_print(WStack *stack);

void *w_stack_get_curr(WStack *stack);
