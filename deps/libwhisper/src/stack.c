#include "whisper/stack.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void w_stack_create(WStack *stack, uint elm_sz, uint num_elms) {
  stack->base_pointer = calloc(elm_sz, num_elms);
  stack->stack_pointer = stack->base_pointer;
  // calc the top of the stack, i think it's faster to do this here and save the
  // result in the ADT header than do it on each push.
  stack->top_pointer =
      (void *)((uint8_t *)stack->base_pointer + (elm_sz * num_elms));
  stack->num_elms = num_elms;
  stack->elm_sz = elm_sz;
}

bool w_stack_at_bottom(WStack *stack) {
  return (stack->stack_pointer == stack->base_pointer);
}

void *w_stack_get_curr(WStack *stack) {
  if (stack->stack_pointer <= stack->base_pointer) {
    return NULL;
  }
  return stack->stack_pointer - stack->elm_sz;
}

void *w_stack_pop(WStack *stack) {
  if (stack->stack_pointer == stack->base_pointer) {
    return NULL;
  }

  stack->stack_pointer = (uint8_t *)stack->stack_pointer - stack->elm_sz;
  return stack->stack_pointer;
}

void *w_stack_push(WStack *stack) {
  if (stack->stack_pointer == stack->top_pointer) {
    return NULL;
  }

  void *return_ptr = stack->stack_pointer;
  stack->stack_pointer = (uint8_t *)stack->stack_pointer + stack->elm_sz;
  return return_ptr;
}

void w_stack_pop_all(WStack *stack) {
  stack->stack_pointer = stack->base_pointer;
}

void w_stack_clean(WStack *stack) {
  free(stack->base_pointer);
  memset(stack, 0, sizeof(WStack));
}

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define PURPLE "\033[35m"

#define RESET "\033[0m"

void w_stack_debug_print(WStack *stack) {
  printf("\n" YELLOW "===Stack Debug Information===" RESET "\n");
  printf(RED "STACK POINTER" RESET "\n");
  printf(GREEN "TOP POINTER" RESET "\n");
  printf(BLUE "BASE POINTER" RESET "\n");
  printf("\t");

  uint8_t *pointer = stack->base_pointer;
  uint counter = 0;
  uint elm_counter = 0;
  while (pointer < (uint8_t *)stack->top_pointer) {
    bool is_element_pointer =
        (((uint8_t *)stack->top_pointer - pointer) % stack->elm_sz == 0);
    if (is_element_pointer) {
      printf("\n\t %d: ", elm_counter);
      elm_counter++;
    }

    for (int i = 0; i < stack->elm_sz; i++) {
      bool is_stack_pointer = (pointer == (uint8_t *)stack->stack_pointer);
      bool is_top_pointer = (pointer == ((uint8_t *)stack->top_pointer - 1));
      bool is_base_pointer = (pointer == (uint8_t *)stack->base_pointer);

      printf("|");

      if (is_stack_pointer) {
        printf(RED);
      } else if (is_top_pointer) {
        printf(GREEN);
      } else if (is_base_pointer) {
        printf(BLUE);
      }

      printf(" 0x%02X ", *pointer);

      if (is_stack_pointer || is_top_pointer || is_base_pointer) {
        printf(RESET);
      }

      pointer += 1;
      counter++;
    }
  }
  printf("\n");
  printf(YELLOW "===End Stack Debug Information===" RESET "\n\n");
}
