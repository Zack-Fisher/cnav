#include "whisper/queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

void w_make_queue(WQueue *wqueue, uint elm_sz, uint num_elms) {
  wqueue->num_elms = num_elms;
  wqueue->elm_sz = elm_sz;
  wqueue->head_idx = 0;
  wqueue->rear_idx = 0;

  wqueue->active_elements = 0;

  // setup ringbuffer.
  wqueue->buffer = calloc(elm_sz, num_elms);
}

void w_queue_save_state(const WQueue *wqueue, WQueueSaveState *dest) {
  dest->head_idx = wqueue->head_idx;
  dest->rear_idx = wqueue->rear_idx;
  dest->active_elements = wqueue->active_elements;
}
void w_queue_load_state(WQueue *wqueue, const WQueueSaveState *from) {
  wqueue->head_idx = from->head_idx;
  wqueue->rear_idx = from->rear_idx;
  wqueue->active_elements = from->active_elements;
}

void *w_enqueue_alloc(WQueue *queue) {
  if (queue->active_elements == queue->num_elms) {
    // dequeue to make more room.
    w_dequeue(queue);
  }

  uint i_elm_offset = queue->elm_sz * queue->rear_idx;
  uint8_t *i_elm_ptr = (uint8_t *)queue->buffer + i_elm_offset;

  queue->rear_idx++;
  queue->rear_idx %= queue->num_elms;

  queue->active_elements++;

  return i_elm_ptr;
}

void w_enqueue(WQueue *queue, const void *data) {
  memcpy(w_enqueue_alloc(queue), data, queue->elm_sz);
}

void *w_dequeue(WQueue *queue) {
  if (queue->active_elements != queue->num_elms &&
      queue->rear_idx == queue->head_idx) {
    return NULL;
  }

  uint index_elm_offset = queue->elm_sz * queue->head_idx;
  uint8_t *index_elm_ptr = (uint8_t *)queue->buffer + index_elm_offset;

  queue->head_idx++;
  queue->head_idx %= queue->num_elms;

  queue->active_elements--;

  // try to use this memory as soon as you dequeue it, safety is not guaranteed
  // and it won't stay forever, it might be bumped out.
  return index_elm_ptr;
}

// helper that pops through all the elements of the queue in a row.
void w_dequeue_all(WQueue *queue) {
  for (;;) {
    if (w_dequeue(queue) == NULL)
      break;
  }
}

void w_clean_queue(WQueue *queue) {
  free(queue->buffer);
  queue->buffer = NULL;
  // don't free the queue, we haven't necessarily alloced that on the heap.
}
