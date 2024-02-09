#include <stdio.h>
#include "../stack.h"


int main()
{
Stack* mystack = Stack_init() ;
char removed ;
for (int i = 0 ; i<10 ; i++)
{
Stack_Push(mystack,(char)i) ;
}
printf("peek = %d\n",Stack_Peek(mystack));
print_list(mystack) ;
removed = Stack_Pop(mystack) ;
printf("peek = %d\n",Stack_Peek(mystack)); 

printf("Stack_Size = %d\n",Stack_Size(mystack)) ;
print_list(mystack) ;
Stack_Free(mystack) ;



}
