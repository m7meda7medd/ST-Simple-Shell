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

/***************************/
extern char **environ;
/****************************/
#define INITIAL_BUF_SIZE 128
#define BUF_SIZE_INC   10

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
    unsigned char Pipe;
} ParserData_t;

typedef struct Char_info {
    char ch;
    unsigned long index;
} char_info_t;

/* functions Declarations */


ReturnStatus GetShellMessage(void);


#endif
