#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#include <functions.h>

void executeCommandBg(char *command, char *argv[])
{
	int forkRet = fork();

	if(forkRet==-1)
	{
		char *err = strerror(errno);
		showShellPrompt(strcat(err, "\n"));
	}
	else if(forkRet==0)
	{
		int ip = open("child_ip", O_WRONLY | O_CREAT, S_IRWXU);
		int op = open("child_op", O_WRONLY | O_CREAT, S_IRWXU);

		dup2(ip, 0);
		dup2(op, 1);

		char *token = strtok(command, " ");

		int i = 0;
		while(token != NULL)
		{
			if(*token != '&')
			{
				argv[i] = token;
				token = strtok(NULL, " ");
				i++;
			}
			else
			{
				break;
			}
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
		char temp[1000] = "Process ID : ";
		char pid[1000];

		sprintf(pid, "%d", forkRet);
		strcat(temp, pid);

		showShellPrompt(strcat(temp, "\n"));
	}
}
