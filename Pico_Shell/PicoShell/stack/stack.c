#include"stack.h"

Stack* Stack_init(void)
{
return list_init() ;
}

unsigned int Stack_Size(Stack* stack){
return (stack->size) ;
}
void Stack_Push(Stack* stack,char data){
list_add_to_index(stack,0,data) ;
}
char Stack_Pop(Stack* stack)
{ char returned_data ;
if (list_remove(stack,0,&returned_data) == -1 )
{
return -1 ;
}else
{

return returned_data ;

}
}

char Stack_Peek(Stack*stack)
{
return list_get(stack,0) ;

}
void Stack_Free(Stack* stack){

free_list(stack) ;
}




