#ifndef _QUEUE_INCLUDED
#define _QUEUE_INCLUDED

typedef struct queue Queue;
typedef void *Item;

/*
 * implementation of a FIFO queue using a linked list
 * ignore priority argument in add()
 */
struct q_element {
  struct q_element *next;
  struct q_element *prev;
  void *value;
};

struct queue {
  struct q_element *head;
  struct q_element *tail;
  int size;
};

/*
 * create a Queue that holds Items
 * returns NULL if the create call failed (malloc failure)
 */
Queue *q_create(void);
/*
 * remove one, add one
 */
int q_shift(Queue *q, Item i);
/*
 * add Item to the Queue
 * return 1/0 if successful/not-successful
 */
int q_add(Queue *q, Item i);
/*
 * remove next Item from queue; returns NULL if queue is empty
 */
Item q_remove(Queue *q);

Item get_at(Queue *q, int i);

Item get_last(Queue *q);

int q_size(Queue *q);

#endif /* _QUEUE_INCLUDED */
