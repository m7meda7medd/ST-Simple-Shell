#ifndef QUEUE_H
#define QUEUE_H
#include "linkedList/ll.h"


typedef List Queue ;



Queue* Queue_init(void) ;
void Queue_enqueue(Queue* queue ,char data) ;
char Queue_dequeue(Queue* queue) ;
char Queue_free(Queue* queue) ;
void Queue_print(Queue* queue) ;
char Queue_front(Queue* queue) ;
unsigned char Queue_isEmpty(Queue* queue);

#endif
