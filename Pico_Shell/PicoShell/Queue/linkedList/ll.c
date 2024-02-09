#include"ll.h"


List* list_init() {
    List* new_list = (List*)malloc(sizeof(List));
    new_list->size = 0;
    new_list->head = NULL;
    return new_list;
}

 void list_add_to_index(List* list,unsigned long index ,char data) {
    	Node* new_node = (Node*) malloc(sizeof(Node));
	new_node->data = data ;
    if (index == 0){
    new_node->next = list->head;
    list->head = new_node;
    }
    else if (index == list->size -1)  
    {	Node* temp_node = list->head  ;    //head -> 1 , 2 ,3 ,4 
	while(temp_node->next != NULL){
	temp_node = temp_node->next ; //head -> 1 -> 2 ->3 
	}
	temp_node->next = new_node ;
	new_node->next = NULL ;
    }
    else
    {
	 Node* temp_node = list->head  ;    
	for (unsigned long i = 0 ; i < index-1 ;i++)
          {
          temp_node = temp_node->next ; //head -> 1 -> 2 ->3 
          }
	new_node->next = temp_node->next ;
        temp_node->next = new_node ;
    }
    list->size++;
}
int list_remove(List* list,unsigned long index,char * removed_data) {
    if (list->size == 0) {
        return -1 ;
    }
    Node* l_node = list->head;
    if (index == 0 )
    {
    *removed_data = l_node->data;
    list->head = l_node->next;
    free(l_node);
    list->size-- ;
    }
    else if (index == list->size-1)
    {
    for (unsigned int i = 0 ; i<index-1 ; i++)
	{
	l_node= l_node->next  ; //head ->1 ->2 ->3 ->4
	}
    *removed_data = l_node->next->data;
    free(l_node->next);
    l_node->next = NULL ;
    list->size--;
    }
    else
    { Node*  node_to_remove = NULL ;
    for (unsigned long i = 0 ; i < index-1 ; i++)
    {
	    l_node =  l_node->next ;    
    }
    node_to_remove = l_node->next ;
    *removed_data = node_to_remove->data;
    l_node->next = node_to_remove->next ;
    free(node_to_remove) ;
    node_to_remove = NULL ;
    list->size--;
    }
    return 0;
}

void free_list(List* list) {
    Node* current_node = list->head;
    Node* next_node = NULL  ; 
    while (current_node != NULL) {
       	next_node = current_node->next;
        free(current_node);
        current_node = next_node;
    }
    free(list);
}
char list_get(List *list , unsigned long index ) {
Node* l_node = list->head ;
if (list->size == 0 )
{
	return -1 ;
}
else
{
		
 for (unsigned int i = 0 ; i<index ; i++)
 {
					// head-> 1 ,2 ,3 ,4
	l_node = l_node->next ;
 }
 
return l_node->data ;
}
}
void print_list(List* list )
{ 
unsigned long  i = 0 ;
Node* temp_node = list->head ;
printf("[") ;
while (temp_node != NULL)
{
if (i == list->size -1)
{
printf("data[%ld] = %d",i,temp_node->data) ; 	
}
else{
printf("data[%ld] = %d, ",i,temp_node->data) ;	
}
temp_node = temp_node->next ;
i++ ;
}
printf("]") ;
}
