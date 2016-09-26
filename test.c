#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

int main()
{
	char temp[1000] = "ls  grep  wc";

	char *token = strtok(temp, "|");

	while(token != NULL)
	{
		printf("%c", *token);
		token = strtok(NULL, " | ");
	}

	printf("\n");
}