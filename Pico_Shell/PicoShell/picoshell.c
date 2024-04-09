#include "picoshell.h"
/* static functions Declarations */
char **Parser(ParserData_t * ParseData);
static int echo(unsigned long argc, char **argv);
static int pwd(void);
static void Store_in_Temp_Queue(Queue ** Env_Queue, char ch);
static int Allocate_Mem_for_Ptr(unsigned long *l_argc, char ***argv);
static int Allocate_Mem_for_Char(unsigned long *l_argc, char ***argv,
				 char_info_t * info);
static void Resolve_Env_Var(char ***argv, unsigned long *l_argc,
			    Queue ** Env_Queue, unsigned long *index);
void Run_command(cmd_typedef * cmd_arr, int cmd_index, int *pipes_arr,
		 int pipes_size);
static void ReapZombies(void);
/****************************************************************/

void SigintHandler(int sig_num)
{
    printf("\n");
}

void SigchldHandler(int sig_num)
{
    ReapZombies();
}

/******************************************************************/
char *local_env = NULL;		// to handle local and enviroment variables 
ReturnStatus GetShellMessage(void)
{
    char *env_user = NULL, **tokens = NULL;
    unsigned long argc;
    ParserData_t ParseData = (ParserData_t) {.argc = 0,.Redirection =
	    0,.pipe = 0
    };
    char *L_H = NULL, *R_H = NULL;
    unsigned long file_index = 0;
    int err, fd, cmd_size = 1,
	pipes_size = 0, pipes_index = 0, *pipes_arr = NULL;
    cmd_typedef *cmd_arr = NULL;
    int new_fd = 0, history_fd = -2, signal_fd = -2, num_write;
    unsigned char background = 0;
    pid_t pid = -1;
    unsigned char redirection = 0, l_pipe = 0;
    char *str_token = NULL;	// used when setting enviroment variable 
    signal(SIGCHLD, SigchldHandler);
    signal(SIGINT, SigintHandler);
    env_user = getlogin();
    ReturnStatus status = STATUS_TRUE;
    printf(ANSI_COLOR_RED "%s" ANSI_COLOR_MAGNETA "@"
	   ANSI_COLOR_BLUE "STM" ANSI_COLOR_MAGNETA "-" ANSI_COLOR_MAGNETA
	   "linux:$ " ANSI_COLOR_RESET, env_user);

    history_fd = open("history.txt", O_CREAT | O_RDWR | O_APPEND, 0666);
    if ((history_fd == -1)) {
	perror("Can't open history file\n");
	exit(EXIT_FAILURE);
    }
    tokens = Parser(&ParseData);
    if (ParseData.argc != 0) {
	if (ParseData.pipe == 1) {
	    cmd_arr =
		(cmd_typedef *) realloc(cmd_arr,
					cmd_size * sizeof(cmd_typedef));
	    cmd_arr[0].cmd = tokens;
	}
	int i = 0;
	while (tokens[i] != NULL) {
	    num_write = write(history_fd, tokens[i], strlen(tokens[i]));
	    if (num_write == -1) {
		perror("Error writing to history file\n");
	    }
	    num_write = write(history_fd, " ", sizeof(char));
	    if ((strchr(tokens[i], '>') != NULL)
		|| (strchr(tokens[i], '<') != NULL)) {
		redirection = 1;
		if ((0 == strcmp(tokens[i], ">")
		     && (file_index = i + 1, new_fd = 1))
		    || ((0 == strcmp(tokens[i], "<"))
			&& ((file_index = i + 1, new_fd = 0) || (1)))) {
		    free(tokens[i]);
		    tokens[i] = NULL;
		    pid = fork();
		    if (pid == -1) {
			perror("Error with Fork\n");
		    } else if (pid == 0) {
			fd = open(tokens[file_index], O_RDWR | O_CREAT,
				  0777);
			if (fd == -1) {
			    perror("can't open File\n");
			} else {
			    if (dup2(fd, new_fd) == -1) {
				perror("Error Occured with dup2\n");
			    } else {
				close(fd);
				if (tokens[0] != NULL) {
				execvp(tokens[0], tokens);
				perror("cmd can't found");
				exit(EXIT_FAILURE);
				}
			    }
			}
		    } else {
			if (tokens[0] != NULL )
			{
			pid = wait(&err);
			}
		    }
		} else if ((0 == strcmp(tokens[i], ">>")
			    && (file_index = i + 1, new_fd = 1))) {
		    free(tokens[i]);
		    tokens[i] = NULL;

		    pid = fork();
		    if (pid == -1) {
			perror("Error with Fork");
		    } else if (pid > 0)	//parent 
		    {
			pid = wait(&err);

		    } else {
			fd = open(tokens[file_index],
				  O_RDWR | O_CREAT | O_APPEND);
			if (fd == -1) {
			    perror("can't open File\n");
			} else {
			    free(tokens[i]);
			    tokens[i] = NULL;
			    if (dup2(fd, new_fd) == -1) {
				perror("Error Occured with dup2\n");
			    } else {
				close(fd);
				execvp(tokens[0], tokens);
				perror("Error with execvp");
				exit(EXIT_FAILURE);
			    }
			}
		    }
		} else {
		    if (tokens[i] != NULL) {
			L_H = strtok(tokens[i], "<>&");
			R_H = strtok(NULL, "<>&");
			if ((L_H != NULL) && (R_H != NULL));
			{
			    pid = fork();
			    if (pid == -1) {
				perror("Error with fork");
				exit(EXIT_FAILURE);
			    } else if (pid == 0) {
				if (dup2(atoi(R_H), atoi(L_H) == -1)) {
				    perror("Error Occured with dup2\n");
				    exit(EXIT_FAILURE);
				} else {
				    free(tokens[i]);
				    tokens[i] = NULL;
				    close(atoi(R_H));
				    L_H = NULL;
				    R_H = NULL;
				    execvp(tokens[0], tokens);
				    exit(EXIT_FAILURE);
				}
			    } else {
				free(tokens[i]);
				tokens[i] = NULL;
				pid = wait(&err);
			    }
			}
		    }
		}
	    } else if (0 == strcmp(tokens[i], "|")) {
		pipes_size++;
		cmd_size++;
		free(tokens[i]);
		tokens[i] = NULL;
		cmd_arr =
		    (cmd_typedef *) realloc(cmd_arr,
					    sizeof(cmd_typedef) *
					    cmd_size);
		cmd_arr[cmd_size - 1].cmd = &(tokens[i + 1]);

	    }
	    i++;
	}
	if (tokens != NULL) {
	    num_write = write(history_fd, "\n", 1);
	}


	if (ParseData.pipe == 1) {
	    pipes_size = 2 * pipes_size;
	    pipes_arr =
		(int *) realloc(pipes_arr, sizeof(int) * (pipes_size));
	    pipes_index = pipes_size - 1;
	    for (int index = 0; index < pipes_size; index += 2)	// adjust pipes values 
	    {
		if (pipe(&(pipes_arr[index])) == -1) {
		    perror("Error with Pipe");
		    exit(EXIT_FAILURE);
		}

	    }
	    pipes_index = pipes_size - 1;
	    int read_index = pipes_index - 1;
	    int write_index = pipes_index;

	    for (int j = cmd_size - 1; j >= 0; j--) {
		if (j == cmd_size - 1) {
		    cmd_arr[j].in = pipes_arr[read_index];
		    cmd_arr[j].out = 1;
		    read_index -= 2;
		} else if (j == 0) {
		    cmd_arr[j].in = 0;
		    cmd_arr[j].out = pipes_arr[write_index];
		} else {
		    cmd_arr[j].in = pipes_arr[read_index];
		    cmd_arr[j].out = pipes_arr[write_index];
		    write_index -= 2;
		    read_index -= 2;
		}
	    }

	    Run_command(cmd_arr, cmd_size - 1, pipes_arr, pipes_size);	// Run command after setting Pipes.

	    for (int ii = 0; ii < pipes_size; ii++) {
		close((pipes_arr[ii]));
	    }
	    for (int ii = 0; ii <= cmd_size - 1; ii++) {
		waitpid(-1, NULL, 0);
	    }
	    free(pipes_arr);
	    free(cmd_arr);
	}
	if ((tokens[ParseData.argc - 1] != NULL)
	    && (!(strcmp(tokens[ParseData.argc - 1], "&")))) {
	    background = 1;
	}
	if ( (tokens[0] != NULL) && (((ParseData.pipe == 0) && (redirection == 0))
	    || (strcmp(tokens[0], "exit") == 0)
	    || (strcmp(tokens[0], "cd") == 0)))  {
	    if (!strcmp(tokens[0], "exit")) {
		status = STATUS_FALSE;
	    } else if (!strcmp(tokens[0], "export")) {
		int status = 0;

		status = putenv(tokens[1]);
		if (status) {
		    perror("Error to Set env var");
		}
	    } else if (!strcmp(tokens[0], "echo")) {
		err = echo(ParseData.argc, tokens);
		if (err != 0) {
		    printf("errno = %d :Error occurred with echo", errno);
		}
	    } else if (!strcmp(tokens[0], "pwd")) {
		err = pwd();
		if (err != 0) {
		    printf("errno = %d :Error occurred with pwd", errno);
		}


	    } else if (0 == strcmp(tokens[0], "cd")) {
		if ((ParseData.argc == 1 /* cd only */ )
		    || (!strcmp(tokens[1], "~"))) {

		    char *home = getenv("HOME");	// get home path from enviroment variables
		    if (home == NULL) {
			perror("can't see Home Directory\n");
			exit(EXIT_FAILURE);
		    } else {
			if (chdir(home)) {
			    perror("Error: Can't change directory\n");
			    exit(EXIT_FAILURE);


			}

		    }
		} else {
		    if (chdir(tokens[1])) {	// return 0 on success , -1 on fail 
			perror("Error occurred with cd\n");
			exit(EXIT_FAILURE);
		    }

		}
	    } else if (!strcmp(tokens[0], "unset")) {
		int status;
		if (ParseData.argc != 2) {
		    perror("Error: too much arguments for unset\n");
		    exit(EXIT_FAILURE);

		} else {
		    status = unsetenv(tokens[1]);
		    if (status) {
			perror("Error with unset\n");
			exit(EXIT_FAILURE);

		    }

		}

	    } else if (strchr(tokens[0], '=') != NULL) {
		str_token = strtok(tokens[0], "=");
		char *value = strtok(0, "\0");
		int status = 0;
		if ((str_token != NULL) && (value != NULL)) {
		    status = setenv(str_token, value, 1);
		    if (status) {
			perror("Error to Set env var ");
			exit(EXIT_FAILURE);
		    }
		}

	    } else {
		pid = fork();
		if (pid > 0)	// Parent 
		{
		    if (!background) {
			pid_t returned_child;
			int wstatus;
			returned_child = wait(&wstatus);
		    }
		} else if (pid == 0)	// child 
		{
		    if (background) {
			free(tokens[ParseData.argc - 1]);
			tokens[ParseData.argc - 1] = NULL;
		    }
		    if (execvp(tokens[0], tokens) == -1);
		    {
			printf("%s : command not found\n", tokens[0]);
			exit(EXIT_FAILURE);
		    }
		} else {

		    perror("Failed to fork\n");
		    exit(EXIT_FAILURE);
		}
	    }
	} else {

	}
    }

    if (history_fd > 0) {
	close(history_fd);
    }
    if (signal_fd > 0) {
	close(signal_fd);
    }
    for (unsigned long i = 0; i < argc; i++) {
	if (tokens[i] != NULL) {
	    free(tokens[i]);
	}
    }

    free(tokens);
    return status;
}

void Run_command(cmd_typedef * cmd_arr, int cmd_index, int *pipes_arr,
		 int pipes_size)
{
    pid_t pid = -1;

    pid = fork();
    if (pid == -1) {
	perror("Error with fork");
	exit(EXIT_FAILURE);
    } else if (pid == 0) {
	int i = 0;
	while ((i <= pipes_size - 1)
	       && (cmd_arr[cmd_index].in != pipes_arr[i])
	       && (cmd_arr[cmd_index].out != pipes_arr[i])) {
	    close(pipes_arr[i]);
	    i++;
	}

	if ((cmd_arr[cmd_index].in) != 0) {
	    close(cmd_arr[cmd_index].in + 1);
	    dup2((cmd_arr[cmd_index].in), 0);
	    close(cmd_arr[cmd_index].in);
	}
	if ((cmd_arr[cmd_index].out) != 1) {
	    //close(cmd_arr[cmd_index].out-1) ;
	    dup2(((cmd_arr[cmd_index]).out), 1);
	    close(cmd_arr[cmd_index].out);
	}
	execvp(cmd_arr[cmd_index].cmd[0], cmd_arr[cmd_index].cmd);
	perror("error with execvp");
	exit(EXIT_FAILURE);
    } else {
	if (cmd_index == 0) {
	    return;
	} else {
	    Run_command(cmd_arr, cmd_index - 1, pipes_arr, pipes_size);
	}
    }
}



char **Parser(ParserData_t * ParseData)
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
	    if (state == START_OF_TOKEN) {
		if (l_argc == 0) {
		    break;
		}
	    }
	    state = END_OF_CMD;
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
		    exit(EXIT_FAILURE);
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
		} else if ((ch2 == '|') && (!special_char)) {
		    ParseData->pipe = 1;
		}
		if (special_char)
		    special_char = 0;

		if (state == IN_TOKEN) {

		    if (env) {

			if ((ch2 == ' ') || (ch2 == '\n') || (ch2 == ':')) {

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
	} else if ((ch == '|') && (!special_char)) {
	    ParseData->pipe = 1;
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
		if (ch == ':') {	// special characters with env var 
		    Resolve_Env_Var(&argv, &l_argc, &Env_Queue, &index);
		    env = 0;
		} else {
		    Store_in_Temp_Queue(&Env_Queue, ch);
		    continue;
		}
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
		if (index != 0) {
		    info = (char_info_t) {
		    .ch = '\0',.index = index};
		    err = Allocate_Mem_for_Char(&l_argc, &argv, &info);
		    if (err == -1) {
			perror("Error Allocating Mem for Char");
			while (1);
		    }
		    index = 0;
		}
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

    ParseData->argc = l_argc;
    return argv;
}

/*  
 * scope : static
 *
 * breif : Function that Reap Zombie Processes 
 *
 * parameters : void  
 * 
 * type : Synchronous 
 *
 * return : void
 *
 */

static void ReapZombies(void)
{
    pid_t pid;
    int wstatus;
    int signal_fd;
    int num_write = 0;
    char msg[100];
    signal_fd = open("log.txt", O_CREAT | O_RDWR | O_APPEND, 0666);
    if (signal_fd == -1) {
	perror("Error Opening log file");
    }
    while ((pid = waitpid(-1, &wstatus, WNOHANG)) > 0) {
	sprintf(msg, "[%d] Terminated with status code = %d\n", pid,
		wstatus);
	num_write = write(signal_fd, msg, strlen(msg));
	if (num_write == -1) {
	    perror("can't write to log.txt\n");
	}
    }
    close(signal_fd);
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
    if ((*Env_Queue) != NULL) {
	Queue_enqueue((*Env_Queue), '\0');
	temp_env =
	    (char *) realloc(temp_env,
			     sizeof(char) * ((*Env_Queue)->size));
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
    while (argv[count] != NULL) {
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
	printf
	    ("errno = %d : Size argument is zero and buf is not null\n",
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
