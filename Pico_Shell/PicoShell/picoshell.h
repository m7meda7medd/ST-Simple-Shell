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


/***************************/
extern char **environ;
/****************************/


/* Configurations */
#define INITIAL_BUF_SIZE 20
#define BUF_SIZE_INC 10
#define ENV_VAR_SIZE 128

/* typedefs */
typedef enum
{
  STATUS_TRUE,
  STATUS_FALSE
} ReturnStatus;
/* functions Declarations */
ReturnStatus GetShellMessage (void);


#endif
