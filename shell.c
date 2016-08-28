#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

char homeDirectory[1000];

void showShellPrompt(char *message)
{
	char curr[1000];
	getcwd(curr, sizeof(curr));

	if(strncmp(curr, homeDirectory, strlen(homeDirectory))==0)
	{
		int homeLen = strlen(homeDirectory);
		int currLen = strlen(curr);

		if(currLen==homeLen)
			strcpy(curr, "~");
		else
		{
			char temp[1000] = "~";

			strcpy(curr, strcat(temp, &curr[homeLen]));
		}
	}

	char uname[1000];
	getlogin_r(uname, 1000);

	char hostname[1000];
	gethostname(hostname, 1000);

	printf("%s[%s@%s:%s] ", message, uname, hostname, curr);
}

void changeDir(char *newDir)
{
	int ret = chdir(newDir);
	int status = 0;

	if(ret == -1)
	{
		char *err = strerror(errno);
		showShellPrompt(strcat(err, "\n"));
	}
	else
	{
		showShellPrompt("");
	}
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
		char usr[1000] = "/usr/";

		char fullPath[1000] = "/bin/";

		char *token = strtok(command, " ");
		strcat(fullPath, token);

		int i = 0;
		while(token != NULL)
		{
			argv[i] = token;
			token = strtok(NULL, " ");
			i++;
		}

		int exeRet = execvp(fullPath, argv);

		if(exeRet==-1)
		{
			int ret = execvp(strcat(usr, fullPath), argv);

			if(ret == -1)
			{
				printf("No command found\n");

				_Exit(0);
			}
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
		dup2(op, 2);

		char usr[1000] = "/usr/";

		char fullPath[1000] = "/bin/";

		char *token = strtok(command, " ");
		strcat(fullPath, token);

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

		int exeRet = execvp(fullPath, argv);

		if(exeRet==-1)
		{
			int ret = execvp(strcat(usr, fullPath), argv);

			if(ret == -1)
			{
				printf("No command found\n");

				_Exit(0);
			}
			else
			{
				FILE *fp = fopen("child_op", "w+");

				dup2(2, 2);

				char temp[1000];
				fgets(temp, 1000, fp);
				printf("%s", temp);
			}
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

int main()
{
	getcwd(homeDirectory, sizeof(homeDirectory));

	char inp[1000];

	printf("\n");
	showShellPrompt("");

	while(1)
	{
		gets(inp);

		if(inp[0] == '\0')
			showShellPrompt("");
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
		else
		{
			char *argv[100];

			if(inp[strlen(inp)-1] == '&')
			{
				executeCommandBg(inp, argv);
			}
			else
				executeCommand(inp, argv);
		}

	}

	printf("\n");

	return 0;
}