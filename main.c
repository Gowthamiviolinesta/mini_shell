/*Name:Gowthami S
Date:-13/02/2026
Description:-minishell project*/

#include<stdio.h>
#include "main.h"

char prompt[100] = "Minishell$ ";
char input_string[100];
int main()
{
	system("clear");
	//char prompt[100]="Minishell$ ";
	//char input_string[100];
	signal(SIGINT, signal_sigint);
    	signal(SIGTSTP, signal_sigtstp);
	scan_input(prompt,input_string);
}
