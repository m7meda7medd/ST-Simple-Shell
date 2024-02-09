#ifndef LL_FILE_H
#define LL_FILE_H

#include <stdio.h>
#include <stdlib.h>
typedef struct node {
    char data;
    struct node * next;
}Node;

typedef struct list {
    unsigned long size;
    Node* head;
}List;

List* list_init() ;
void list_add_to_index(List* list,unsigned long index ,char data) ;
int list_remove(List* list,unsigned long index,char * removed_data) ;
void free_list(List* list) ;
char list_get(List *list , unsigned long index ) ;
void print_list(List* list) ;
#endif

