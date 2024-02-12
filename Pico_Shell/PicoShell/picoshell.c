#include "picoshell.h"
/* static functions Declarations */
static char **Parser(unsigned long *);
static int echo(unsigned long argc, char **argv);
static int pwd(void);
static void Store_in_Temp_Queue(Queue ** Env_Queue, char ch);
static int Allocate_Mem_for_Ptr(unsigned long *l_argc, char ***argv);
static int Allocate_Mem_for_Char(unsigned long *l_argc, char ***argv,
				 char_info_t * info);
static void Resolve_Env_Var(char ***argv, unsigned long *l_argc,
			    Queue ** Env_Queue, unsigned long *index);
/****************************************************************/
char *local_env = NULL;		// to handle local and enviroment variables 
ReturnStatus GetShellMessage(void)
{
    char *env_user = NULL, **tokens = NULL;
    unsigned long argc;
    ParserData_t ParserData;
    int err;
    pid_t pid = -1;
    char *str_token = NULL;	// used when setting enviroment variable 
    env_user = getlogin();
    ReturnStatus status = STATUS_TRUE;
    printf("%s@stm-linux:$ ", env_user);
    tokens = Parser(&argc);

    if (argc != 0) {
	if (0 == strcmp(tokens[0], "exit")) {
	    status = STATUS_FALSE;
	} else if (0 == strcmp(tokens[0], "echo")) {
	    err = echo(argc, tokens);
	    if (err != 0) {
		printf("errno = %d :Error occurred with echo", errno);
	    }
	} else if (0 == strcmp(tokens[0], "pwd")) {
	    err = pwd();
	    if (err != 0) {
		printf("errno = %d :Error occurred with pwd", errno);
	    }


	} else if (0 == strcmp(tokens[0], "cd")) {
	    if (argc != 2) {
		printf("Usage of cd is: cd path\n");
	    } else {
		if (chdir(tokens[1]) == 0) {
		    // done 
		} else {
		    printf("errno = %d:Error occurred with cd\n", errno);
		}
	    }

	} else if (0 == strcmp(tokens[0], "unset")) {
	    int status;
	    if (argc != 2) {
		printf("Error");

	    } else {
		status = unsetenv(tokens[1]);
		if (status) {
		    printf("Can't Unset: err:%d\n", errno);

		}

	    }

	} else if (strchr(tokens[0], '=') != NULL) {
	    str_token = strtok(tokens[0], "=");
	    char *value = strtok(0, "\0");
	    int status = 0;
	    status = setenv(str_token, value, 1);
	    if (status) {
		printf("Error to Set %s", str_token);
	    }


	} else {
	    pid = fork();
	    if (pid > 0)	// Parent 
	    {
		pid_t returned_child;
		int wstatus;
		unsigned char res_wstatus = 0;
		returned_child = wait(&wstatus);

		/*  // For Debugging Purposes 
		   res_wstatus = ((unsigned char) (WIFEXITED(wstatus) << 0) |
		   (unsigned char) (WEXITSTATUS(wstatus) << 1) |
		   (unsigned char) (WIFSIGNALED(wstatus) << 2) | (unsigned char) (WTERMSIG(wstatus) << 3) | (unsigned char) (WCOREDUMP(wstatus) << 4 ) | (unsigned char) (WIFSTOPPED(wstatus)<<5) | (unsigned char) (WSTOPSIG(wstatus) << 6) | (unsigned char) (WIFCONTINUED(wstatus) << 7)) ;
		   switch (res_wstatus){ // for futute edit 
		   case 1  :
		   printf("res_wstatus = %d\n",res_wstatus) ;break ;

		   case 2 :
		   printf("res_wstatus = %d\n",res_wstatus) ;
		   break ;

		   case 4 : printf("res_wstatus = %d\n",res_wstatus)  ;break ;

		   case 8 : printf("res_wstatus = %d\n",res_wstatus)  ;break ;

		   case 16 : printf("res_wstatus = %d\n",res_wstatus)  ;break ;

		   case 32 : printf("res_wstatus = %d\n",res_wstatus)  ;break ;

		   case 64 : printf("res_wstatus = %d\n",res_wstatus) ; break ;

		   case 128 :
		   printf("res_wstatus = %d\n",res_wstatus) ; 
		   break ; 
		   default : 
		   printf("Error with wait result \n") ;        
		   }
		 */

	    } else if (pid == 0)	// child 
	    {
		if (execvp(tokens[0], tokens) == -1);
		{
		    // printf("%s : Can't execute with errno = %d\n",tokens[0],errno) ;
		    printf("%s :command not found\n", tokens[0]);
		}

	    } else {

		printf("Failed to fork\n");
	    }
	}
    } else {

    }
    for (unsigned long i = 0; i < argc; i++)
	free(tokens[i]);
    free(tokens);
    return status;
}



char **Parser(unsigned long *argc)
{
    parse_state state = START_OF_TOKEN;
    size_t index = 0;
    char ch, ch2, special_char = 0, env = 0;
    int err = 0;
    unsigned l_size = 0;
    Queue *Env_Queue = NULL;
    char **argv = NULL;
    unsigned long l_argc = 0;
    char_info_t info;		// number of tokens
    fflush(stdin);		// fflush stdin to avoid buffer undefined behaviour 
    while (ch = getchar())	// loop on characters entered in stdin
    {

	if ((ch == '\n')) {
	    if (state == IN_TOKEN) {
		state = END_OF_CMD;
	    } else {
		break;
	    }
	} else if (ch == ' ') {
	    if (state == IN_TOKEN) {
		state = END_OF_TOKEN;
	    } else {
		continue;
	    }

	} else if (((ch == '\"') || (ch == '\'')) && (!special_char)) {

	    if (state == START_OF_TOKEN) {

		err = Allocate_Mem_for_Ptr(&l_argc, &argv);
		if (err == -1) {
		    perror("Error Allocating Mem for Char");
		    while (1);
		}
		state = IN_TOKEN;
		index = 0;
	    }
	    while ((ch2 = getchar())) {
		if (ch2 == '\n') {
		    printf(">");

		}
		if ((ch2 == ch) && (!special_char)) {
		    state = END_OF_QUOTES;

		} else if ((ch2 == '\\') && (!special_char)) {
		    special_char = 1;
		    continue;
		} else if ((ch2 == '$') && (!special_char)) {

		    env = 1;
		    continue;
		}
		if (special_char)
		    special_char = 0;

		if (state == IN_TOKEN) {

		    if (env) {

			if ((ch2 == ' ') || (ch2 == '\n')) {

			    Resolve_Env_Var(&argv, &l_argc, &Env_Queue,
					    &index);
			    env = 0;
			} else {

			    Store_in_Temp_Queue(&Env_Queue, ch2);
			    continue;
			}
		    }
		    // save ch2 in token 

		    info = (char_info_t) {
		    .ch = ch2,.index = index};
		    err = Allocate_Mem_for_Char(&l_argc, &argv, &info);
		    if (err == -1) {

			perror("Error Allocating Mem for Char");
		    }
		    index++;
		} else if (state == END_OF_QUOTES) {

		    if (env) {
			Resolve_Env_Var(&argv, &l_argc, &Env_Queue,
					&index);
			env = 0;
		    }
		    break;
		}
	    }
	} else if ((ch == '\\') && (special_char == 0)) {
	    special_char = 1;
	    continue;
	} else if ((ch == '$') && (special_char == 0)) {
	    env = 1;
	    continue;
	}

	if (special_char)
	    special_char = 0;

	if (state == START_OF_TOKEN)	// allocate mem for ptr and store char 
	{
	    err = Allocate_Mem_for_Ptr(&l_argc, &argv);
	    if (err == -1) {
		perror("Error Allocating Mem for Ptr\n");
	    }

	    if (env) {
		Store_in_Temp_Queue(&Env_Queue, ch);
		state = IN_TOKEN;
		continue;
	    }

	    info = (char_info_t) {
	    .ch = ch,.index = index};
	    err = Allocate_Mem_for_Char(&l_argc, &argv, &info);
	    if (err == -1) {
		perror("Error Allocating Mem for Char\n");
		while (1);
	    }
	    index++;
	    state = IN_TOKEN;
	} else if (state == IN_TOKEN) {

	    if (env) {
		Store_in_Temp_Queue(&Env_Queue, ch);
		continue;

	    }
	    info = (char_info_t) {
	    .ch = ch,.index = index};
	    err = Allocate_Mem_for_Char(&l_argc, &argv, &info);
	    if (err == -1) {
		perror("Error Allocating Mem for Char\n");
		while (1);
	    }

	    index++;
	} else if (state == END_OF_QUOTES) {
	    state = IN_TOKEN;
	    continue;
	} else			// case end of cmd or token 
	{

	    if (env) {
		Resolve_Env_Var(&argv, &l_argc, &Env_Queue, &index);
		env = 0;
	    }

	    if (state == END_OF_TOKEN) {
		info = (char_info_t) {
		.ch = '\0',.index = index};
		err = Allocate_Mem_for_Char(&l_argc, &argv, &info);
		if (err == -1) {
		    perror("Error Allocating Mem for Char");
		    while (1);
		}
		index = 0;
		state = START_OF_TOKEN;
	    } else {		// case end of cmd 

		info = (char_info_t) {
		.ch = '\0',.index = index};
		err = Allocate_Mem_for_Char(&l_argc, &argv, &info);
		if (err == -1) {
		    perror("Error Allocating Mem for Char");
		    while (1);
		}
		index = 0;

		err = Allocate_Mem_for_Ptr(&l_argc, &argv);
		l_argc--;
		if (err == -1) {
		    perror("Error Allocating Mem for Ptr");
		    while (1);
		}
		break;
	    }
	}

    }

    *argc = l_argc;
    return argv;
}

/*  
 * scope : static
 *
 * breif : Function Stores the Name of Env variable in Temporary Queue
 *
 * parameters : IN/OUT : Queue** Env_Queue , IN : char ch  
 * 
 * type : Synchronous 
 *
 * return : void
 *
 */

static void Store_in_Temp_Queue(Queue ** Env_Queue, char ch)
{
    if ((*Env_Queue) == NULL) {
	(*Env_Queue) = Queue_init();
	Queue_enqueue((*Env_Queue), ch);
    } else {

	Queue_enqueue((*Env_Queue), ch);
    }
}

/*
 * type : static 
 *
 * breif : Function Resolves the Enviroment variable and if found store it in it's token
 * else do nothing with it 
 *
 * parameters : IN/OUT : Queue*** argv ,unsigned long* l_argc,
 * Queue** Env_Queue , unsigned long* index , IN : char ch
 *
 * return : void
 */


static void Resolve_Env_Var(char ***argv, unsigned long *l_argc,
			    Queue ** Env_Queue, unsigned long *index)
{

    unsigned int i = 0;
    char *temp_env = NULL;
    char *l_str = NULL;
    size_t str_len;

    Queue_enqueue((*Env_Queue), '\0');
    temp_env =
	(char *) realloc(temp_env, sizeof(char) * ((*Env_Queue)->size));
    while (!Queue_isEmpty(*Env_Queue)) {
	temp_env[i++] = Queue_dequeue(*Env_Queue);
    }
    if ((l_str = getenv(temp_env)) != NULL) {
	str_len = strlen(l_str);
	(*argv)[(*l_argc) - 1] =
	    (char *) realloc((*argv)[(*l_argc) - 1],
			     sizeof(char) * (*index + str_len + 1));
	strncpy(&((*argv)[(*l_argc) - 1][*index]), l_str, str_len);
	*index += str_len;
    } else {
	// enviroment variable can't be found
    }

    free(temp_env);
    temp_env = NULL;
    l_str = NULL;
    Queue_free(*Env_Queue);
    *Env_Queue = NULL;
}

/*  
 * scope : static
 *
 * breif : Function allocates ptr dynamically 
 *
 * parameters : IN/OUT : unsigned long* l_argc , char*** argv. 
 * 
 * type : Synchronous 
 *
 * return : int err  in case of fail allocation err = -1 
 *                    else err = 0 
 *
 */

static int Allocate_Mem_for_Ptr(unsigned long *l_argc, char ***argv)
{
    int err = 0;
    (*l_argc)++;		// increase number of tokens by one
    (*argv) = (char **) realloc((*argv), sizeof(char *) * (*l_argc));	// allocate space for the new pointer to the token
    if ((*argv) == NULL) {
	err = -1;
    }
    (*argv)[(*l_argc) - 1] = NULL;	// avoid dangling pointer :D
    return err;
}

static int Allocate_Mem_for_Char(unsigned long *l_argc, char ***argv,
				 char_info_t * info)
{
    int err = 0;
    (*argv)[(*l_argc) - 1] = (char *) realloc((*argv)[(*l_argc) - 1], sizeof(char) * (info->index + 1));	//alocate memory for the character .
    if ((*argv)[(*l_argc) - 1] == NULL) {
	err = -1;
    }				// err = -1
    (*argv)[(*l_argc) - 1][info->index] = info->ch;	// store the char
    return err;
}

/*  
 * scope : static
 *
 * breif : Built in echo command Function 
 *
 * parameters : IN/OUT : char*** argv , IN : unsigned long argc
 * 
 * type : Synchronous 
 *
 * return :   in case of fail err = -1  "fail"
 *                    else err = 0 "Succcess"
 *
 */


static int echo(unsigned long argc, char **argv)	// echo implementation 
{
    unsigned char count = 1;
    int num_write = 0;
    char ch;
    while (count != argc) {
	num_write = write(1, argv[count], strlen(argv[count]));	// echo the arg
	if (num_write == -1)	// check if there is an error happened
	{
	    return -1;
	}
	count++;
	if (count != argc)
	    num_write = write(1, " ", 2);	// echo space between each ar

	if (num_write == -1)	// check if there is an error happened
	{
	    return -1;
	}
    }

    num_write = write(1, "\n", 2);	// echo space between each argument
    return 0;
}

/*  
 * scope : static
 *
 * breif : Built in pwd command Function 
 *
 * parameters : void 
 * 
 * type : Synchronous 
 *
 * return :   in case of fail err = -1  "fail" and errno updated with err value 
 *                    else err = 0 "Succcess"
 *
 */


static int pwd(void)
{
    size_t buf_size = INITIAL_BUF_SIZE;
    char *buf = NULL;

    buf = getcwd(buf, buf_size);

    if (buf == (char *) NULL) {
	errno = ENOMEM;
    }
    while (((char *) NULL == buf) && (errno == ERANGE))	// if there is a range error happened and  NULL returned
    {
	free(buf);
	buf = NULL;
	buf_size += BUF_SIZE_INC;
	buf = getcwd(buf, buf_size);

    }
    switch (errno) {
    case EACCES:
	printf("errno = %d : Permision Denied\n", errno);
	return -1;
	break;
    case EFAULT:
	printf("errno = %d : Bad Address \n", errno);
	return -1;
	break;
    case EINVAL:
	printf("errno = %d : Size argument is zero and buf is not null\n",
	       errno);
	return -1;
	break;
    case ENOENT:
	printf("errno = %d : Unlinked Dir \n", errno);
	return -1;

	break;
    case ENOMEM:
	printf("errno = %d : Out of Memory \n", errno);
	return -1;
	break;
    default:
	printf("%s\n", buf);
    }
    free(buf);
    return 0;
}
