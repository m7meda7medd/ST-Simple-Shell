#include "queue.h" 

Queue* Queue_init(void) 
{
return list_init() ;

}
void Queue_enqueue(Queue* queue ,char data)
{
list_add_to_index(queue,0,data) ;
}

char Queue_dequeue(Queue* queue) {
char removed_data ;
int status   ;
if (queue->size == 0)
{
	return -1 ;
}
else
{
list_remove(queue,queue->size-1,&removed_data) ;
return removed_data ;
} 	
}
char Queue_free(Queue* queue){

free_list(queue) ;
}
void Queue_print(Queue* queue)
{
print_list(queue) ;
}
unsigned char Queue_isEmpty(Queue* queue)
{
if (queue->size == 0)
{
return 1 ;	
}
else
{
	return 0 ;
}
}
char Queue_front(Queue* queue)
{  
	return list_get(queue , queue->size-1) ;
}
