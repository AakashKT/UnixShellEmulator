#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#include <functions.h>


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
		char *token = strtok(command, " ");

		int i = 0;
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