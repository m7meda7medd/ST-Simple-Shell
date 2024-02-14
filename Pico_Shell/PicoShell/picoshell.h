#ifndef PICO_SHELL_H
#define PICO_SHELL_H
/* includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "Queue/queue.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/***************************/
extern char **environ;
/****************************/
#define INITIAL_BUF_SIZE 128
#define BUF_SIZE_INC   10



#define ANSI_COLOR_RED "\e[31m"
#define ANSI_COLOR_GREEN "\e[32m"
#define ANSI_COLOR_BLUE "\e[34m"
#define ANSI_COLOR_RESET "\e[0m"
#define ANSI_COLOR_MAGNETA "\e[35m"

/* typedefs */
typedef enum {
    STATUS_TRUE,
    STATUS_FALSE
} ReturnStatus;

typedef enum Parse_stat {
    START_OF_TOKEN,
    IN_TOKEN,
    END_OF_TOKEN,
    END_OF_CMD,
    END_OF_QUOTES
} parse_state;

typedef struct data_from_Parser {
    unsigned long argc;
    unsigned char Redirection;
    unsigned char pipe;
} ParserData_t;

typedef struct Char_info {
    char ch;
    unsigned long index;
} char_info_t;

typedef struct cmd_struct {
char** cmd ;
int in ; 
int out ;
}cmd_typedef ;
/* functions Declarations */


ReturnStatus GetShellMessage(void);


#endif
