#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include "main.h"

extern pid_t pid;
extern char prompt[100];
extern char input_string[];
Job *job_head=NULL;
extern int status;
int next_job_no = 1;
/*creats a new job entry for a stopped process and appends it to the end of the job list*/
void insert_job_last(pid_t pid, char *input_string)
{
    Job *new = malloc(sizeof(struct job));
    if (!new)
        return;

    new->pid = pid;
    new->job_no = next_job_no++;
    strcpy(new->cmd, input_string);
    new->next = NULL;

    if (job_head == NULL)
    {
        job_head = new;
        return;
    }

    Job *temp = job_head;
    while (temp->next)
        temp = temp->next;

    temp->next = new;
}
/*Handle Ctrl+C by printing the prompt for the shell*/
void signal_sigint(int sig_num)
{
	if ( sig_num == SIGINT )
	{
	if(pid == 0)
	{
		//parent
		//print the prompt in the terminal
		printf("\n%s",prompt);
		fflush(stdout);
	}
	}
	else
	{
		waitpid(-1,&status,WNOHANG);
	}
	

}
/* Handles Ctrl+Z by stopping foreground process and adding it to the job list*/
void signal_sigtstp(int sig_num)
{
	if(pid > 0)
	{
		printf("\n[%d] stopped\t%s\n",next_job_no,input_string);
		fflush(stdout);
		insert_job_last(pid,input_string);
	}
	else if(pid == 0)
	{
		printf("\n%s",prompt);
		fflush(stdout);
	}

}
/*Display all currently stopped jobs stored in job list*/
void print_jobs(void)
{
    Job *temp = job_head;

    if (!temp)
    {
        //printf("No stopped jobs\n");
        return;
    }

    while (temp)
    {
        printf("[%d] Stopped\t%s\n",temp->job_no,temp->cmd);
        temp = temp->next;
    }
}
/*Removes most recently added job from the list*/
void delete_last_job(void)
{
    if (!job_head)
        return;

    if (!job_head->next)
    {
        //free(job_head);
        job_head = NULL;
        return;
    }

    Job *temp = job_head;
    while (temp->next->next)
        temp = temp->next;

    //free(temp->next);
    temp->next = NULL;
}
/*returns pid of most recently added job from the job list*/
pid_t get_last_job_pid(void)
{
    if (!job_head)
        return -1;

    Job *temp = job_head;
    while (temp->next)
        temp = temp->next;

    return temp->pid;
}
/*Finds and returns the job structure corresponding to a given pid*/
Job *get_job_by_pid(pid_t pid)
{
    Job *temp = job_head;
    while (temp) {
        if (temp->pid == pid)
            return temp;
        temp = temp->next;
    }
    return NULL;
}
