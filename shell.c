#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#include <functions.h>

char homeDirectory[1000];

int main()
{
	getcwd(homeDirectory, sizeof(homeDirectory));

	printf("\n");
	showShellPrompt("");

	while(1)
	{
		char inp[1000];
		gets(inp);

		if(inp[0] == '\0')
		{
			showShellPrompt("");
		}
		else if(strncmp(inp, "exit", 4)==0)
		{
			printf("Logout successful\n\n");
			_Exit(0);
		}
		else if(strncmp(inp, "cd", 2)==0)
		{	
			changeDir(&inp[3]);
		}	
		else if(strncmp(inp, "pwd", 3)==0)
		{
			char temp[1000];
			getcwd(temp, sizeof(temp));

			showShellPrompt(strcat(temp, "\n"));
		}
		else if(strncmp(inp, "echo", 4)==0)
		{
			showShellPrompt(strcat(&inp[5], "\n"));
		}
		else if(strncmp(inp, "pinfo", 5)==0)
		{
			char *argv[100];
			char psCommand[100] = "ps -l -x";

			char *token = strtok(inp, " ");
			token = strtok(NULL, " ");

			if(token != NULL)
			{
				char flag[10] = " ";

				strcat(psCommand, flag);
				strcat(psCommand, token);
			}

			executeCommand(psCommand);
		}
		else if(strncmp(inp, "listjobs", 8)==0)
		{
			printPoolProc();
			showShellPrompt("");
		}
		else if(strncmp(inp, "killallbg", 9)==0)
		{
			deleteAllFromPool();
			showShellPrompt("");
		}
		else if(strncmp(inp, "quit", 4)==0)
		{
			_Exit(0);
		}
		else
		{
			if(inp[strlen(inp)-1] == '&')
				executeCommandBg(inp);
			else
				executeCommand(inp);
		}

		int stat;
		int retPid = waitpid(-1, &stat, WNOHANG);
		if(WIFEXITED(stat)==1 || WIFSIGNALED(stat)==1 || WIFSTOPPED(stat)==1)
			deleteFromPool(retPid);

	}

	printf("\n");

	return 0;
}