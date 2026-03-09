#include<stdio.h>
#include "main.h"
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/wait.h>

Job *job_list=NULL; 
char *external_commands[153];
int status;
pid_t pid;
/*Updates the shell prompt string based on the user-defined PS1 value*/
void copy_change(char *prompt, char *input_string)
{
    // copy input_string into prompt
    strcpy(prompt, input_string); 
    strcat(prompt,"$ ");
   	
}
/*Extracts and returns the first word (command) from the input string*/
char* get_command(char *input_string)
{
	//copy the string upto the space
	static char str[100]; 
    	int i = 0;

    	while (input_string[i] != ' ' && input_string[i] != '\0')
    	{
        	str[i] = input_string[i];
        	i++;
    	}
    	str[i] = '\0';

    	return str;

}
/*Implements special echo variables like $$, $?, and $SHELL.*/
void echo(char *input_string)
{
	if(strcmp(input_string,"echo $$") == 0)
	{
		//print process_id
		printf("%d\n",getpid());
	}
	else if(strcmp(input_string,"echo $?") == 0)
	{
		//print exitstatus
		printf("%d\n",WEXITSTATUS(status));
	}
	else if(strcmp(input_string,"echo $SHELL")==0)
	{
		//print minishell executable path
		char* str=getenv("SHELL");
		printf("%s\n",str);
	}
}
/*Determines whether a command is builtin, external, or invalid*/
int check_command_type(char *command)
{
	if (command == NULL || *command == '\0')
        return NO_COMMAND;

    	for (int i = 0; builtins[i]; i++)
    	{
        	if (strcmp(command, builtins[i]) == 0)
            	return BUILTIN;
    	}

    	for (int i = 0; external_commands[i]; i++)
    	{
        	if (strcmp(command, external_commands[i]) == 0)
            	return EXTERNAL;
    	}

    return NO_COMMAND;
}
/*Executes builtin shell commands such as pwd, cd, exit, and echo*/
void execute_internal_commands(char *input_string)
{
	char buffer[100];
	//check the input_string is pwd or not
	if(strcmp(input_string,"pwd") == 0)
	{
		//get the current directory
		char* pwd = getcwd(buffer,sizeof(buffer));
		printf("%s\n",pwd);
	}
	else if(strncmp(input_string,"cd",2) == 0)
	{
		//change the current directory
		//check with pwd whether cd is changed or not
		char* path = input_string + 2;
		while(*path == ' ')
			path++;
		if(*path == '\0')
			chdir(getenv("HOME"));
		else
		if(chdir(path) != '\0')
			perror("cd");
	}
	else if(strcmp(input_string,"exit") == 0)
	{
		exit(0);
	}
	else if(strncmp(input_string,"echo",4) == 0)
	{
		//special variables
		echo(input_string);
	}

}
/*Executes multiple external commands connected using pipes (|).*/
void n_pipe(char** argv,int pipe_count)
{
	

	int index[pipe_count+1];
	int j=0;
	index[j]=0;
	for(int i=0;argv[i]!=NULL;i++)
	{
		if(strcmp(argv[i],"|")==0)
		{
			argv[i]=NULL;
			index[++j]=i+1;
		}

	}
	for(int i=0;i<pipe_count+1;i++)
	{
		int pipe_fd[2];
		if(i != pipe_count)
			pipe(pipe_fd);

		int pid = fork();
		if(pid > 0)
		{
			if(i != pipe_count)
			{
				close(pipe_fd[1]);
				dup2(pipe_fd[0],0);
			}

		}
		else if(pid == 0)
		{
			if(i != pipe_count)
			{
				close(pipe_fd[0]);
				dup2(pipe_fd[1],1);
			}
			execvp(argv[index[i]],argv+index[i]);
		}
		else
		{
			perror("fork:");
		}
	}
	for(int i=0;i<=pipe_count;i++)
	{
		wait(&status);
		//printf("child%d terminated with %d exit status\n",i,WEXITSTATUS(status));
	}

}
/*executes an external command with or without pipes.*/
void execute_external_commands(char *input_string)
{
	//convert the inputstring into 2-D array
	int i=0,j=0,index=0;
	char buffer[100];
	char* argv[50];
	while(1)
	{
		if(input_string[i] == ' ' || input_string[i] == '\0')
		{
			buffer[j]='\0';
			argv[index]=malloc(j+1);
			strcpy(argv[index],buffer);
			index++;
			j=0;
			if (input_string[i] == '\0')
            			break; // end of string
			i++;
		}
		else
		{
			buffer[j++]=input_string[i++];
		}

	}
	argv[index]=NULL;

	//take the backup of stdin,stdout
	int fd_out = dup2(1,3);
	int fd_in = dup2(0,4);
	//check the pipe symbol is present in 2-D array or not
	int pipe_count=0;
	for(int k=0;argv[k];k++)
	{
		if(strcmp(argv[k],"|") == 0)
		{
			pipe_count++;
		}
	}
	if(pipe_count == 0)
	{
		//execute the command using execvp()
		execvp(argv[0],argv+0);
		perror("execvp");
		exit(1);
	}
	else
	{
		//call n-pipe function
		n_pipe(argv,pipe_count);
	}
	//restore stdin stdout
	/*dup2(fd_in,0);
	dup2(fd_out,1);
	close(fd_in);
	close(fd_out);*/
}
/*copy commands from .txt file to 2-D array*/
void extract_external_commands(char **external_commands)
{
	//open the file
	int fd = open("external_cmd.txt",O_RDONLY);
	if(fd == -1)
		return;
	char buff[100];
	int i=0,cmd_index=0;
	char ch;
	while(read(fd,&ch,1) == 1)
	{
		if(ch == '\n')
		{	
			buff[i] = '\0';
			external_commands[cmd_index]=malloc(i+1);
			strcpy(external_commands[cmd_index],buff);
			cmd_index++;
			i=0;
		}
		else
		{
			buff[i++]=ch;
		}
	}
	if (i > 0)  // leftover text after EOF
	{
    		buff[i] = '\0';
    		external_commands[cmd_index] = malloc(i + 1);
    		strcpy(external_commands[cmd_index], buff);
    		cmd_index++;
	}
	external_commands[cmd_index] = NULL;
	close(fd);
}
void scan_input(char* prompt,char* input_string)
{
	//extract external commands
	extract_external_commands(external_commands);

	while(1)
	{
		printf("%s",prompt);
		memset(input_string,'\0',100);
		if(fgets(input_string,100,stdin) == NULL)
			continue;
		//scanf(" %[^\n]",input_string);

		int len=strlen(input_string);
		if(len > 0 && input_string[len-1]=='\n')
			input_string[len-1]='\0';
		//skip empty input
		if(input_string[0] == '\0')
		{
			continue;
		}
		/*check the PS1 variable*/
		if(strncmp(input_string,"PS1=",4)==0)
		{
			if(strncmp(input_string+4," ",1)==0)
			{
				printf("no command\n");
				continue;
			}
			char* new_prompt = input_string+4;
			/*validate the PS1 variable*/
			if(strlen(new_prompt) > 0)
			{
				copy_change(prompt,new_prompt);
			}
			else
			{
				printf("command not found\n");
			}
			continue;
		}
		char* command=get_command(input_string);
		
		if(command == NULL)
			continue;
		int ret=check_command_type(command);

		if(ret == BUILTIN)
		{
			if (strcmp(command, "jobs") == 0)
                        {
                                print_jobs();
                                continue;
                        }
                        else if(strcmp(command, "fg") == 0)
                        {
                                pid_t pid = get_last_job_pid();
                      
                                if (pid > 0)
                                {
					Job *job = get_job_by_pid(pid);

        				if (job != NULL)
        				{
            					printf("[%d] %s\n", job->job_no, job->cmd);
            					fflush(stdout);
        				}
                                        kill(pid, SIGCONT);
                                        waitpid(pid, &status, WUNTRACED);

                                        if (!WIFSTOPPED(status))
                                        delete_last_job();
                                }
                                continue;

                        }
                        else if(strcmp(command, "bg") == 0)
                        {
                                pid_t pid = get_last_job_pid();
                                if (pid > 0)
				{
					Job *job = get_job_by_pid(pid);

                                        if (job == NULL)
                                        {
                                                continue;
                                        }
					printf("[%d] %s &\n", job->job_no, job->cmd);
    					fflush(stdout);
                                	kill(pid, SIGCONT);
					signal(SIGCHLD,signal_sigint);
                                        delete_last_job();
					//signal(SIGCHILD,signal_sigtstp);
				}

                        }

			//execute internal command
			execute_internal_commands(input_string);
		}
		else if(ret == EXTERNAL)
		{
			pid = fork();
			//create child process
			if(pid == 0)
			{
				//make the both signal as default
				signal(SIGINT, SIG_DFL);
				signal(SIGTSTP, SIG_DFL);

				//execute external command

                        	execute_external_commands(input_string);	
				exit(0);
			}
			else if(pid > 0)
			{
				//wait for child
				//wait(&status);
				
				waitpid(pid,&status,WUNTRACED);

			pid = 0;
			
			}
		}
		else
		{
			//NO Command
			printf("no command\n");
		}
	}
}
