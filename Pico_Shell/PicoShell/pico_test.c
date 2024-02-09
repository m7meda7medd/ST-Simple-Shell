#include "picoshell.h"
/* static functions Declarations */
static char **Parser (unsigned long *);
static int echo (int argc, char **argv);
static int pwd (void);
/*****************************************/
ReturnStatus
GetShellMessage (void)
{
  char *env_user = NULL, **tokens = NULL;
  unsigned long argc;
  int err;
  pid_t pid = -1;
  char *str_token = NULL;	// used when setting enviroment variable 
  env_user = getlogin ();
  ReturnStatus status = STATUS_TRUE;
  printf ("%s@stm-linux:$ ", env_user);
  tokens = Parser (&argc);
  if (argc != 0)
    {
      if (0 == strcmp (tokens[0], "exit"))
	{
	  status = STATUS_FALSE;
	}
      else if (0 == strcmp (tokens[0], "echo"))
	{
	  err = echo (argc, tokens);
	  if (err != 0)
	    {
	      printf ("errno = %d :Error occurred with echo", errno);
	    }
	}
      else if (0 == strcmp (tokens[0], "pwd"))
	{
	  err = pwd ();
	  if (err != 0)
	    {
	      printf ("errno = %d :Error occurred with pwd", errno);
	    }


	}
      else if (0 == strcmp (tokens[0], "cd"))
	{
	  if (argc != 2)
	    {
	      printf ("Usage of cd is: cd path\n");
	    }
	  else
	    {
	      if (chdir (tokens[1]) == 0)
		{
		  // done 
		}
	      else
		{
		  printf ("errno = %d:Error occurred with cd\n", errno);
		}
	    }

	}
      else if (0 == strcmp (tokens[0], "unset"))
	{
	  int status;
	  if (argc != 2)
	    {
	      printf ("Error");

	    }
	  else
	    {
	      status = unsetenv (tokens[1]);
	      if (status)
		{
		  printf ("Can't Unset: err:%d\n", errno);

		}

	    }

	}
      else if (strchr (tokens[0], '=') != NULL)
	{
	  str_token = strtok (tokens[0], "=");
	  char *value = strtok (0, "\0");
	  int status = 0;
	  status = setenv (str_token, value, 0);
	  if (status)
	    {
	      printf ("Error to Set %s", str_token);
	    }


	}
      else
	{
	  pid = fork ();
	  if (pid > 0)		// Parent 
	    {
	      pid_t returned_child;
	      int wstatus;
	      unsigned char res_wstatus = 0;
	      returned_child = wait (&wstatus);

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

	    }
	  else if (pid == 0)	// child 
	    {
	      if (execvp (tokens[0], tokens) == -1);
	      {
		// printf("%s : Can't execute with errno = %d\n",tokens[0],errno) ;
		printf ("%s :command not found\n", tokens[0]);
	      }

	    }
	  else
	    {

	      printf ("Failed to fork\n");
	    }
	}
    }
  else
    {

    }
  for (unsigned long i = 0; i < argc; i++)
    free (tokens[i]);
  free (tokens);
  return status;
}

char **
Parser (unsigned long *argc)
{
  size_t index = 0;
  char ch;
  char ch2;
  int status = 0;
  unsigned l_size = 0;
  char quotes = 0;
  int env = 0;
  char *l_str = NULL;
  char **argv = NULL;
  unsigned char start = 1;	// start ==  1 when we will start new token
  unsigned long l_argc = 0;	// number of tokens
  fflush (stdin);		// fflush stdin to avoid buffer undefined behaviour 
  while (ch = getchar () != '\n')	// loop on characters entered in stdin
    {
      if (ch != ' ')		// if char is not space 
	{
	  if (start)		// if it's about to start new token
	    {
	      l_argc++;		// increase number of tokens by one
	      argv = (char **) realloc (argv, sizeof (char *) * l_argc);	// allocate space for the new pointer to the token 
	      if (argv == NULL)	// if it can't allocate print and break
		{
		  // printf ("Can't Allocate = 1");
		  break;
		}
	      argv[l_argc - 1] = NULL;	// avoid dangling pointer :D
	      start = 0;
	    }

	  if ((ch == '\"') || (ch == '\''))
	    {
	      quotes = 1;
	      while ((ch2 = getchar ()) != ch)
		{
		  if (ch2 == '\n')
		    {
		      printf ("> ");
		    }
		  if (ch2 == ch)
		    {
		      quotes = 0;
		    }
		  if (quotes == 1)
		    {
		      argv[l_argc - 1] = (char *) realloc (argv[l_argc - 1], sizeof (char) * (index + 1));	//alocate memory for the character .
		      if (argv[l_argc - 1] == NULL)	// if it can't allocate print and break
			{
			  // printf ("Can't Allocate = 2"); 
			  break;
			}
		      argv[l_argc - 1][index] = ch2;	// store the char
		      index++;
		    }
		}
	      start = 0;
	    }
	  else if ((ch == '$') && (0 == index))	//resolving enviroment variables 
	    {
	      env = 1;
	    }
	  else
	    {
	      argv[l_argc - 1] = (char *) realloc (argv[l_argc - 1], sizeof (char) * (index + 1));	//alocate memory for the character .
	      if (argv[l_argc - 1] == NULL)	// if it can't allocate print and break
		{
		  // printf ("Can't Allocate = 2"); 
		  break;
		}
	      argv[l_argc - 1][index] = ch;	// store the char
	      index++;
	    }
	}
      else if (!(start))	// if start == 0 and ch == space , allocate space for ch and store '\0'
	{
	  start = 1;
	  argv[l_argc - 1] =
	    (char *) realloc (argv[l_argc - 1], sizeof (char) * (index + 1));
	  if (argv[l_argc - 1] == NULL)	// same as above xD 
	    {
	      //  printf ("Can't Allocate = 3");
	      break;
	    }
	  argv[l_argc - 1][index] = '\0';
	  if (env == 1)
	    {
	      l_str = NULL;
	      l_str = getenv (argv[l_argc - 1]);
	      if (l_str != NULL)
		{
		  char size = strlen (l_str);
		  argv[l_argc - 1] =
		    (char *) realloc (argv[l_argc - 1],
				      sizeof (char) * (size + 1));
		  strncpy (argv[l_argc - 1], l_str, size);
		  argv[l_argc - 1][size] = '\0';
		}
	      env = 0;
	    }
	  index = 0;		// reset index
	}
      else			// else  space and start == 1 
	{
	  // do nothing 
	  // don't be like this else :D
	}
    }
  if (!(start))			// after looping if start == 0 put '\0' 
    {
      start = 1;
      argv[l_argc - 1] =
	(char *) realloc (argv[l_argc - 1], sizeof (char) * (index + 1));
      if (argv[l_argc - 1] == NULL)	// same as above xD
	{
	  //  printf ("Can't Allocate = 3");
	}
      argv[l_argc - 1][index] = '\0';
      if (env == 1)
	{
	  l_str = NULL;
	  l_str = getenv (argv[l_argc - 1]);
	  if (l_str != NULL)
	    {
	      char size = strlen (l_str);
	      argv[l_argc - 1] =
		(char *) realloc (argv[l_argc - 1],
				  sizeof (char) * (size + 1));
	      strncpy (argv[l_argc - 1], l_str, size);
	      argv[l_argc - 1][size] = '\0';
	    }
	  env = 0;
	}
      index = 0;		// reset index
    }



  argv = (char **) realloc (argv, sizeof (char *) * l_argc + 1);	// allocate space for the new pointer to the token  
  argv[l_argc] = NULL;		// Make The Last Pointer Pointing to NULL argv[argc] == NULL :D

  *argc = l_argc;
  return argv;
}

static int
echo (int argc, char **argv)	// echo implementation 
{
  unsigned char count = 1;
  int num_write = 0;
  char ch;
  while (count != argc)
    {
      if (argv[count][0] == '$')
	{
	  char *env_val = getenv (&argv[count][1]);
	  if (env_val != NULL)
	    {
	      num_write = write (1, env_val, strlen (env_val));	// echo the arg
	    }
	}
      else
	{
	  num_write = write (1, argv[count], strlen (argv[count]));	// echo the arg
	}
      num_write = write (1, " ", 2);	// echo space between each arg 
      if (num_write == -1)	// check if there is an error happened
	{
	  return -1;
	}
      count++;
    }

  num_write = write (1, "\n", 2);	// echo space between each argument
  return 0;
}


static int
pwd (void)			// built in pwd 
{
  size_t buf_size = INITIAL_BUF_SIZE;
  char *buf = NULL;
  if (buf == (char *) NULL)
    {
      errno = ENOMEM;
    }
  buf = getcwd (buf, buf_size);
  while (((char *) NULL == buf) && (errno == ERANGE))	// if there is a range error happened and  NULL returned
    {
      free (buf);
      buf = NULL;
      buf_size += BUF_SIZE_INC;
      buf = getcwd (buf, buf_size);

    }
  switch (errno)
    {
    case EACCES:
      printf ("errno = %d : Permision Denied\n", errno);
      break;
    case EFAULT:
      printf ("errno = %d : Bad Address \n", errno);
      break;
    case EINVAL:
      printf ("errno = %d : Size argument is zero and buf is not null\n",
	      errno);
      break;
    case ENOENT:
      printf ("errno = %d : Unlinked Dir \n", errno);
      break;
    case ENOMEM:
      printf ("errno = %d : Out of Memory \n", errno);
      break;
    default:
      printf ("%s\n", buf);
    }
  free (buf);
  return 0;
}
