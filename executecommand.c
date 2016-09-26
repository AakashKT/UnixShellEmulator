#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#include <functions.h>

int check(char command[1000], char *commands[100])
{
	int total = 0;

	char *token = strtok(command, "|");

	while(token != NULL)
	{
		commands[total] = token;
		total++;

		token = strtok(NULL, "|");
	}

	return total;
}

void executeCommand(char *command, char *argv[])
{
	int forkRet = fork();
	int status = 0;
	if(forkRet==-1)
	{
		char *err = strerror(errno);
		showShellPrompt(strcat(err, "\n"));
	}
	else if(forkRet==0)
	{
		char *commands[100];
		int discreet = check(command, commands);

		int i=0;
		for(i=0;i<discreet;i++)
		{
			char *str = commands[i];

			//printf("%s\n", str);

			char *token = strtok(str, " ");
			int i = 0;
			while(token != NULL)
			{
				argv[i] = token;
				token = strtok(NULL, " ");
				i++;
			}

			int forkRet = fork();
			if(forkRet==-1)
			{
				char *err = strerror(errno);
				showShellPrompt(strcat(err, "\n"));
			}
			else if(forkRet==0)
			{	
				int exeRet = execvp(argv[0], argv);
				if(exeRet==-1)
				{
					printf("No command found\n");

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
		}
		/*
		char *token = strtok(command, " ");

		i = 0;
		while(token != NULL)
		{
			argv[i] = token;
			token = strtok(NULL, " ");
			i++;
		}

		int exeRet = execvp(argv[0], argv);

		if(exeRet==-1)
		{
			printf("No command found\n");

			_Exit(0);
		}
		*/
		showShellPrompt("");
	}
	else
	{
		while(waitpid(-1, NULL, 0))
		{	
			if(errno == ECHILD)
				break;
		}
		showShellPrompt("");
	}
}