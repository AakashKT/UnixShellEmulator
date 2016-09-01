#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#include <functions.h>


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
