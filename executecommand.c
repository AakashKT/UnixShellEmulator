#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include <functions.h>

int check(char command[1000], char *commands[100])
{
	int total = 0;

	char *token = strtok(command, "|");

	while(token != NULL)
	{
		commands[total] = (char*) malloc(100*sizeof(char));
		strcpy(commands[total], token);
		total++;

		token = strtok(NULL, "|");
	}

	return total;
}

void spawnCommand(int in, int out, char *argv[1000])
{
	int forkRet = fork();

	if(forkRet == -1)
	{
		char *err = strerror(errno);
		showShellPrompt(strcat(err, "\n"));
	}
	else if(forkRet == 0)
	{
		dup2(in, STDIN_FILENO);
		dup2(out, STDOUT_FILENO);

		int exeRet = execvp(argv[0], argv);
		if(exeRet == -1)
		{
			char *err = strerror(errno);
			printf("%s\n", err);
			_Exit(0);
		}
	}
	else
	{
		while(waitpid(-1, NULL, 0))
		{
			if(errno == ECHILD)
				break;
		}
	}

	return;
}

void splitParams(char *command, char *argv[1000])
{
	char *token = strtok(command, " ");
	int j=0;
	while(token != NULL)
	{
		argv[j] = (char*) malloc(100*sizeof(char));
		strcpy(argv[j], token);
		token = strtok(NULL, " ");
		j++;
	}
	argv[j] = NULL;

	return;
}

void executeCommand(char inp[1000])
{
	char *distinctCommands[100];
	char *argv[1000];
	int totalDistinct = check(inp, distinctCommands);

	int pipeFd[2];
	int in = STDIN_FILENO;
	int i;

	pipe(pipeFd);

	for(i=0;i<=totalDistinct-2;i++)
	{
		char *command = distinctCommands[i];
		char *argvv[1000];

		splitParams(command, argvv);
		spawnCommand(in, pipeFd[1], argvv);
		in = pipeFd[0];
	}

	splitParams(distinctCommands[totalDistinct-1], argv);
	spawnCommand(in, STDOUT_FILENO, argv);

	showShellPrompt("");

	return;
}