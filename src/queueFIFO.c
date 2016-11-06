#include "queue.h"
#include <stdlib.h>
#include <string.h>


/*
 * Create a Queue that holds Items.
 * returns NULL if the create call failed (malloc failure)
 */
Queue *q_create(void) {
  struct queue *q;

  if ((q = (struct queue *) malloc(sizeof(struct queue))) != NULL) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
  }
  return q;
}

/*
 * In order to keep only a certain amount of items in the
 * queue at any one time, remove one before adding if needed.
 */
int q_shift(Queue *q, Item i) {
  if(q_size(q) >= 40) {
    q_remove(q);
  }
  return q_add(q, i);
}

/*
 * Add Item to the Queue; 3rd arg is priority in MIN_PRIO..MAX_PRIO;
 * return 1/0 if successful/not-successful
 */
int q_add(Queue *q, Item i) {
  struct q_element *p;
  int old_size = q->size;

  p = (struct q_element *) malloc(sizeof(struct q_element));
  if (p != NULL) {
    p->value = i;
    p->next = NULL;
    // If the queue is empty, then don't assign a prev
    // for the first element
    if (q->head == NULL) {
      q->head = p;
    } else {
      q->tail->next = p;
    }

    if(q->tail == NULL) {
      p->prev = NULL;
    } else {
      p->prev = q->tail;
    }
    q->tail = p;
    q->size = old_size + 1;
    return 1;
  }
  return 0;
}

/*
 * remove next Item from queue; returns NULL if queue is empty
 */
Item q_remove(Queue *q) {
  struct q_element *p;
  int old_size = q->size;
  Item i;

  if (q->head == NULL) {
    q->tail = NULL;
    return NULL;
  }
  p = q->head;
  q->head = p->next;
  if(q->head != NULL) {
    q->head->prev = NULL; /* first should not have prev */
  }
  q->size = old_size-1;
  i = p->value;
  free(p);
  return i;
}

Item get_at(Queue *q, int i) {
  struct q_element *p;
  Item item;
  int start;

  if (q->head == NULL) {
    return NULL;
  }
  p = q->head;

  for(start = 0; start < i; start ++) {
    if(p->next==NULL) {
      return NULL;
    }
    p = p->next;
  }

  item = p->value;
  return item;
}

Item get_last(Queue *q) {
  struct q_element *p;
  Item i;

  if(q->tail == NULL) {
    return NULL;
  }
  p = q->tail;
  i = p->value;
  return i;
}

/* Optimization: Keep the size of the list as an int instead
 * of iterating over it to find out the size
 */
int q_size(Queue *q) {
  return q->size;
}
