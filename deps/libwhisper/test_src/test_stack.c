#include "whisper/macros.h"
#include "whisper/stack.h" // Assuming that's where your ADT is defined
#include <stdio.h>
#include <string.h>

void test_stack() {
  WStack stack;

  // Test 1: Create a stack of integers with room for 5 elements
  w_stack_create(&stack, sizeof(int), 5);
  ASSERT(stack.elm_sz == sizeof(int));
  ASSERT(stack.num_elms == 5);

  w_stack_debug_print(&stack);

  // Test 2: Stack push operation
  int val1 = 1, val2 = 2;
  int *p_val1 = (int *)w_stack_push(&stack);
  int *p_val2;

  w_stack_debug_print(&stack);

  ASSERT(p_val1 != NULL);
  *p_val1 = val1;

  w_stack_debug_print(&stack);

  p_val2 = (int *)w_stack_push(&stack);
  ASSERT(p_val2 != NULL);
  *p_val2 = val2;

  w_stack_debug_print(&stack);

  // Test 3: Stack pop operation
  int *pop_val = (int *)w_stack_pop(&stack);
  ASSERT(pop_val);
  ASSERT(*pop_val == val2);

  pop_val = (int *)w_stack_pop(&stack);
  ASSERT(pop_val && *pop_val == val1);

  // Test 4: Pop from empty stack
  pop_val = (int *)w_stack_pop(&stack);
  ASSERT(pop_val == NULL);

  // Test 5: Pop all elements
  w_stack_push(&stack);
  w_stack_push(&stack);
  w_stack_pop_all(&stack);

  pop_val = (int *)w_stack_pop(&stack);
  ASSERT(pop_val == NULL);
}
