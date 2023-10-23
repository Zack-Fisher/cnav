#include "whisper/queue.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int compare_ints(void *a, void *b) { return *((int *)a) == *((int *)b); }

void test_queue() {
  WQueue q;
  w_make_queue(&q, sizeof(int), 5);
  int values[] = {10, 20, 30, 40, 50};
  for (int i = 0; i < 11; i++) {
    printf("queue indices: head %d, rear %d; value %d\n", q.head_idx,
           q.rear_idx, values[i % 5]);
    w_enqueue(&q, &values[i % 5]);
  }

  WQueueSaveState state;
  w_queue_save_state(&q, &state);

  for (int i = 0; i < 3; i++) {
    int *dequeue_val = (int *)w_dequeue(&q);
    assert(compare_ints(dequeue_val, &values[((i + 1) % 5)]));
  }

  // now, the dequeues should start from the same modulus.
  w_queue_load_state(&q, &state);

  for (int i = 0; i < 3; i++) {
    int *dequeue_val = (int *)w_dequeue(&q);
    assert(compare_ints(dequeue_val, &values[((i + 1) % 5)]));
  }

  // pop until the end of the queue, now.
  for (int i = 0; i < 2; i++) {
    int *dequeue_val = (int *)w_dequeue(&q);
    assert(compare_ints(dequeue_val, &values[((i + 4) % 5)]));
  }

  // Test for empty queue
  assert(w_dequeue(&q) == NULL);

  w_clean_queue(&q);
}
