// author kaixiang li
// date   2020/11/12/ 14:45

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUILTIN_COMMANDS 5

char PWD[1024];
char PATH[1024];

//
char * Built_in[] = {"cd", "exit", "help", "pwd", "echo"};

int Built_in_cd(char ** args)
{
  if(args[1] == NULL)
  {
    fprintf(stderr, "minishell: one argument required\n");
  }
  else if (chdir(args[1]) < 0)
  {
     perror("minishell");
  }
  getcwd(PWD, sizeof(PWD));
  return 1;
}

int Built_in_exit(char ** args)
{
  return 0;
}

int Built_in_help(char ** args)
{
  printf("\n                minishell                \n");
  printf("                            by lkx         \n");
  printf("                            by ysy         \n");
  printf("                            by csb         \n");
  printf("\nCommands list: \n");
  printf("\t-> ls\n");
  printf("\t-> cp\n");
  printf("\t-> mv\n");
  printf("\t-> rm\n");
  printf("\t-> vi\n");
  printf("\t-> cd\n");
  printf("\t-> pwd\n");
  printf("\t-> cat\n");
  printf("\t-> help\n");
  printf("\t-> exit\n");
  printf("\t-> echo\n");
  printf("\t-> clear\n");
  printf("\t-> mkdir\n");
  printf("\t-> rmdir\n");
  // printf("\t-> ping\n");
  printf("\n\n");
  printf("\n\t* You know I/O redirection is supported");
  printf("\n\t* (<, <<, >, >>, 2>, 2>>)");
  printf("\n\t* Example: ls -i >> outfile 2> error_file\n");
  return 1;
}

int Built_in_pwd(char ** args)
{
  printf("%s\n", PWD);
  return 1;
}

int Built_in_echo(char ** args)
{
  int i = 1;
  while (1)
  {
    if (args[i] == NULL)
    {
      break;
    } 
    printf("%s ", args[i]);
    i++;
  }
  printf("\n");
}

int (* Built_in_function[]) (char **) = 
{
  &Built_in_cd,
  &Built_in_exit,
  &Built_in_help,
  &Built_in_pwd,
  &Built_in_echo,
};

char ** Command_split_line(char * command)
{
  int position = 0;
  int no_of_takens = 64;
  char ** tokens = malloc(sizeof(char *) * no_of_takens);
  char delim[2] = " ";

  char * token = strtok(command, delim);
  while (token != NULL) 
  {
    tokens[position] = token;
    position++;
    token = strtok(NULL, delim);
  }
  tokens[position] = NULL;
  return tokens;
}

char * Command_read_line(void)
{
  int position = 0;
  int buf_size = 1024;
  char * command = (char *) malloc(sizeof(char) * 1024);
  char c;

  c = getchar();
  while (c != EOF && c != '\n')
  {
    command[position] = c;

    if (position >= buf_size)
    {
      buf_size += 64;
      command = realloc(command, buf_size);
    }

    position++;
    c = getchar();
  }
  return command;
}

int process_start(char ** args)
{
  int status;
  pid_t pid, w_pid;

  pid = fork();

  if(pid == 0)
  {
    char command_dir[1024];
    strcpy(command_dir, PATH);
    strcat(command_dir, args[0]);

    if(execv(command_dir, args) == -1)
    {
      perror("minishell");
    }
    exit(EXIT_FAILURE);
  }
  else if (pid < 0)
  {
    perror("minishell");
  }
  else
  {
    do
    {
      w_pid = waitpid(pid, &status, WUNTRACED);
    }
    while (!WIFEXITED(status) && !WIFSIGNALED(status));
    
  }
  
  return 1;
}

int shell_execute(char ** args)
{
  if(args[0] == NULL)
  {
    return 1;
  }

  int std_in;
  int std_out;
  int std_err;

  std_in = dup(0);
  std_out = dup(0);
  std_err = dup(2);

  int i = 1;

  while (args[i] != NULL)
  {
    if(strcmp(args[i], "<") == 0)
    {
      int in_buf = open(args[i+1], O_RDONLY);
      if(in_buf < 0)
      {
        perror("minishell");
        return 1;
      }
      if(dup2(in_buf, 0) < 0)
      {
        perror("minishell");
        return 1;
      }

      close(in_buf);
      args[i] = NULL;
      args[i+1] = NULL;
      i += 2;
    }
    else if(strcmp(args[i], "<<") == 0)
    {
      int in_buf = open(args[i+1], O_RDONLY);
      if(in_buf < 0)
      {
        perror("minishell");
        return 1;
      }

      if(dup2(in_buf, 0) < 0)
      {
        perror("minishell");
        return 1;
      }
      close(in_buf);
      args[i] = NULL;
      args[i+1] = NULL;
      i += 2;
    }
    else if(strcmp(args[i], ">") == 0)
    {
      int out_buf =open(args[i+1], O_WRONLY | O_TRUNC | O_CREAT, 0755);
      if(out_buf < 0)
      {
        perror("minishell");
        return 1;
      }
      if (dup2(out_buf, 1) < 0)
      {
        perror("minishell");
        return 1;
      }

      close(out_buf);
      args[i] = NULL;
      args[i+1] = NULL;
      i+=2;
    }
    else if(strcmp(args[i], ">>") == 0)
    {
      int out_buf = open(args[i+1], O_WRONLY | O_APPEND | O_CREAT, 0755);
      if(out_buf < 0)
      {
        perror("minishell");
        return 1;
      }
      if(dup2(out_buf, 1) < 0)
      {
        perror("minishell");
        return 1;
      }

      close(out_buf);
      args[i] = NULL;
      args[i+1] = NULL;
      i += 2;
    }
    else if(strcmp(args[i], "2>") == 0)
    {
      int error_buf = open(args[i+1], O_WRONLY | O_CREAT, 0755);
      if(error_buf < 0)
      {
        perror("minishell");
        return 1;
      }
      if(dup2(error_buf, 2) < 0)
      {
        perror("minishell");
        return 1;
      }

      close(error_buf);
      args[i] = NULL;
      args[i+1] = NULL;
      i += 2;
    }
    else if(strcmp(args[i], "2>>") == 0)
    {
			int error_buf = open( args[i+1], O_WRONLY | O_CREAT | O_APPEND, 0755 );

			if ( error_buf < 0 ){
				perror("minsh");
				return 1;
			}

			if ( dup2(error_buf, 2) < 0 ){
				perror("minsh");
				return 1;
			}

			close(error_buf);
			args[i] = NULL;
			args[i+1] = NULL;
			i += 2;
		}
    else
    {
      i++;
    } 
  }

  for (i = 0; i < BUILTIN_COMMANDS; i++)
  {
    if(strcmp(args[0], Built_in[i]) == 0)
    {
      int ret_status = (* Built_in_function[i])(args);

      dup2(std_in, 0);
      dup2(std_out, 1);
      dup2(std_err, 2);

      return ret_status;
    }
  }
  
  int ret_status = process_start(args);

  dup2(std_in, 0);
  dup2(std_out, 1);
  dup2(std_err, 2);
  return ret_status;
}
void loop(void)
{
  int status = Built_in_help(NULL);

  char *command_line;
  char **arguments;

  status = 1;

  while (status)
  {
    printf("%s ->", PWD);
    command_line = Command_read_line();
    if(strcmp(command_line, "") == 0)
    {
      continue;
    }
    arguments = Command_split_line(command_line);
    status = shell_execute(arguments);
  }
  
}
int main(int argc, int ** argv)
{
  getcwd(PWD, sizeof(PWD));
  strcpy(PATH, PWD);
  strcat(PATH, "/commands/");
  loop();
  return 0;
}

