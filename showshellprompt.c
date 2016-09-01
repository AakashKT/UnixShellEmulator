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