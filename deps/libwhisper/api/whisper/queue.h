#pragma once

typedef unsigned int uint;

// a simple generic queue implemented over a ring buffer.

typedef struct WQueue {
  uint elm_sz;
  uint num_elms;

  // instead of a linked list, we're just using direct indices into the ring
  // buffer.
  uint head_idx;
  uint rear_idx;

  uint active_elements; // how many slots are in active use?

  // the queue/ringbuffer data.
  void *buffer;
} WQueue;

typedef struct WQueueSaveState {
  uint head_idx;
  uint rear_idx;
  uint active_elements;
} WQueueSaveState;

void w_make_queue(WQueue *wqueue, uint elm_sz, uint num_elms);

// WARNING: any enqueue call may invalidate your savestate, as that's the only
// ADT method that changes the state of the actual buffer.
// the savestate memory is entirely caller-managed, like usual.
void w_queue_save_state(const WQueue *wqueue, WQueueSaveState *dest);
void w_queue_load_state(WQueue *wqueue, const WQueueSaveState *from);

// put into the back of the queue. if there's already something there,
// dequeue that value and make room for more stuff.
void w_enqueue(WQueue *queue, const void *data);
// just return the pointer to the slot, don't make the caller pass the data
// up-front.
void *w_enqueue_alloc(WQueue *queue);

// pop from the head of the queue, return the pointer to the element.
// return NULL if there are no elements.
void *w_dequeue(WQueue *queue);

void w_dequeue_all(WQueue *queue);

void w_clean_queue(WQueue *queue);
