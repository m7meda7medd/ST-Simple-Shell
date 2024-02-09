#ifndef STACK_H
#define STACK_H
#include "linkedList/ll.h"



typedef List Stack ;


Stack* Stack_init(void) ;
unsigned int Stack_Size(Stack* stack); 
void Stack_Push(Stack* stack,char data) ;
char Stack_Pop(Stack* stack);
char Stack_Peek(Stack*stack) ;
void Stack_Free(Stack*stack) ;
#endif
