#include <stdio.h>
#include "queue.h"

char** Convert_Format(Queue** , unsigned long ) ;
char** Parser (unsigned long* argc);
int main (void)
{	
unsigned long argc ; 
char ** argv = NULL ;
while (1)
{ 


argv = Parser(&argc) ;
if (argc == 0 )
        continue ;
for (unsigned long  i = 0 ; i<argc ; i++)
        printf("%s ",argv[i]) ;

for (unsigned long  j = 0 ; j<argc ; j++)
        free(argv[j]) ;

printf("\n") ;
free(argv) ;

argv = NULL ;


}
}


char** Parser (unsigned long* argc)
{
char ch  ;
unsigned long l_argc = 0 ;
char start = 1 ;
char quotes = 0 ;
char ch2 ;
Queue**  argv_q = NULL ;
while ((ch = getchar()) != '\n')
{
if (ch != ' ')
{
   if (start)
   {
   l_argc++;
   argv_q = (Queue**)realloc(argv_q,sizeof(Queue*) * l_argc) ;
   argv_q[l_argc-1] = Queue_init() ;
   if (argv_q[l_argc-1] == NULL)   
   {	
	  printf("Error\n") ;
    break ;
   }
   start = 0 ;
   }
   //********** Handling Special Characters **************
   /*  " and  ' */
 if ((ch == '\"') || (ch == '\''))
 {  quotes = 1; 
  while ((ch2 = getchar()) != ch)
  {  
	if (ch2 == '\n')
	  {
		printf("> ") ;
	  }
	if (ch2 == ch)
	{
	quotes = 0 ;
	}

	if (quotes == 1)
	{
	Queue_enqueue(argv_q[l_argc-1],ch2); 
	}
  }
 }
 else
 {
   Queue_enqueue(argv_q[l_argc-1],ch) ;
 }
}
   else if (!start)
   {
    start = 1 ;
    Queue_enqueue(argv_q[l_argc-1],'\0') ;
   }
else
{
	//
}
}
if (!start)
{
    start = 1 ;
    Queue_enqueue(argv_q[l_argc-1],'\0') ;
}
  argv_q = (Queue **) realloc (argv_q, sizeof(Queue *) * l_argc + 1); 
  argv_q[l_argc] = NULL ;
  
  *argc = l_argc ;
char** argv = NULL ;
argv = Convert_Format(argv_q , l_argc) ;

char i = 0 ;
while (argv_q[i] != NULL)
        Queue_free(argv_q[i++]) ;
free(argv_q) ;
return argv ; 
}


char** Convert_Format(Queue** Q_arr, unsigned long argc)
{
unsigned long i = 0, ii = 0 ;
char** v_args = (char**)malloc(sizeof(char*)*(argc+1)) ;
while (Q_arr[i] != NULL)
{
ii  = 0 ;
v_args[i] = (char*) malloc(sizeof(char)* Q_arr[i]->size) ;
while (!Queue_isEmpty(Q_arr[i]))
{
v_args[i][ii] = Queue_dequeue(Q_arr[i]) ;
ii++ ;
}
v_args[i][ii] = '\0' ;
i++ ;
}
v_args[argc] = NULL ;

return v_args ;
}
 
