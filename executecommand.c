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

list *rootNode;

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

void spawnCommand(int *in, int *out, char *argv[1000])
{
	int forkRet = fork();

	if(forkRet == -1)
	{
		char *err = strerror(errno);
		showShellPrompt(strcat(err, "\n"));
	}
	else if(forkRet == 0)
	{
		dup2(*in, STDIN_FILENO);
		dup2(*out, STDOUT_FILENO);

		int exeRet = execvp(argv[0], argv);
		if(exeRet == -1)
		{
			char *err = strerror(errno);
			printf("%s\n", err);
			close(*in);
			close(*out);

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

int splitParams(char *command, char *argv[1000], char *redirection[5])
{
	char *token = strtok(command, " ");
	int j=0;
	int isRedirected = -1;
	while(token != NULL)
	{
		if(strcmp(token, "<")==0)
		{
			token = strtok(NULL, " ");
			redirection[0] = (char*) malloc(100*sizeof(char));
			strcpy(redirection[0], token);

			isRedirected++;
		}
		else if(strcmp(token, ">")==0)
		{
			token = strtok(NULL, " ");
			redirection[1] = (char*) malloc(100*sizeof(char));
			strcpy(redirection[1], token);

			isRedirected += 2;
		}
		else if(strcmp(token, ">>")==0)
		{
			token = strtok(NULL, " ");
			redirection[1] = (char*) malloc(100*sizeof(char));
			strcpy(redirection[1], token);

			isRedirected += 4;
		}
		else if(strcmp(token, "&")!=0)
		{
			argv[j] = (char*) malloc(100*sizeof(char));
			strcpy(argv[j], token);
			j++;
		}
		token = strtok(NULL, " ");
	}
	argv[j] = NULL;

	return isRedirected;
}

void setFds(int *in, int *out, int isRedirected, char *redirection[5])
{
	if(isRedirected == 0)
		*in = open(redirection[0], O_RDWR, S_IRWXU);
	else if(isRedirected == 1)
		*out = open(redirection[1], O_RDWR | O_TRUNC, S_IRWXU);
	else if(isRedirected == 3)
		*out = open(redirection[1], O_RDWR | O_APPEND, S_IRWXU);
	else if(isRedirected == 2 || isRedirected == 4)
	{
		*in = open(redirection[0], O_RDWR, S_IRWXU);
		*out = open(redirection[1], O_RDWR | O_TRUNC, S_IRWXU);
	}
}

void executeCommandBg(char inp[1000])
{
	int forkRet = fork();

	if(forkRet == -1)
	{
		char *err = strerror(errno);
		showShellPrompt(strcat(err, "\n"));
	}
	else if(forkRet == 0)
	{		
		int ip = open("child_ip", O_WRONLY | O_CREAT, S_IRWXU);
		int op = open("child_op", O_WRONLY | O_CREAT, S_IRWXU);

		dup2(ip, 0);
		dup2(op, 1);

		executeCommand(inp);

		_Exit(0);
	}
	else
	{
		char temp[1000] = "Process ID : ";
		char pid[1000];

		sprintf(pid, "%d", forkRet);
		strcat(temp, pid);

		addToPool(inp, forkRet);

		showShellPrompt(strcat(temp, "\n"));
	}
}

void executeCommand(char inp[1000])
{
	char *distinctCommands[100];
	char *argv[1000];
	char *redirection[5];
	int totalDistinct = check(inp, distinctCommands);

	int pipeFd[2];
	int in = STDIN_FILENO;
	int out = STDOUT_FILENO;
	int i;

	pipe(pipeFd);

	for(i=0;i<=totalDistinct-2;i++)
	{
		char *command = distinctCommands[i];
		char *argvv[1000];
		char *redirectionInner[5];

		out = pipeFd[1];

		int isRedirected = splitParams(command, argvv, redirectionInner);
		setFds(&in, &out, isRedirected, redirectionInner);

		if(in == -1 || out == -1)
		{
			printf("No such file exists\n");
			showShellPrompt("");

			return;
		}

		spawnCommand(&in, &out, argvv);
		close(pipeFd[1]);

		in = pipeFd[0];
	}

	out = STDOUT_FILENO;

	int isRedirected = splitParams(distinctCommands[totalDistinct-1], argv, redirection);
	setFds(&in, &out, isRedirected, redirection);

	if(in == -1 || out == -1)
	{
		printf("No such file exists\n");
		showShellPrompt("");

		return;
	}

	spawnCommand(&in, &out, argv);

	if(out != STDOUT_FILENO)
		close(out);
	if(in != STDIN_FILENO)
		close(in);

	close(pipeFd[0]);
	showShellPrompt("");

	return;
}