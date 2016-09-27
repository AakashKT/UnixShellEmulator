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

void addToPool(char inp[1000], int pid)
{
	if(rootNode == NULL)
	{
		rootNode = (list*) malloc(sizeof(list));

		rootNode->prev = NULL;
		rootNode->next = NULL;

		rootNode->pid = pid;
		strcpy(rootNode->pname, inp);
	}
	else
	{
		list *temp = rootNode;
		
		while(1==1)
		{
			if(temp->next == NULL)
				break;
			else
				temp = temp->next;
		}

		list *new = (list*) malloc(sizeof(list));
		temp->next = new;
		new->prev = temp;

		temp = temp->next;
		temp->next = NULL;

		temp->pid = pid;
		strcpy(temp->pname, inp);
	}
}

void deleteFromPool(int pid)
{
	list *temp = rootNode;

	if(temp == NULL)
		return;

	while(1==1)
	{
		if(temp->pid == pid)
		{
			list *tempNext = temp->next;
			list *tempPrev = temp->prev;

			if(tempPrev == NULL && tempNext == NULL)
			{
				free(rootNode);
				rootNode = NULL;
				return;
			}

			if(tempPrev != NULL)
				tempPrev->next = tempNext;
			else
				rootNode = tempNext;
	
			if(tempNext != NULL)
				tempNext->prev = tempPrev;

			temp = NULL;
			break;
		}

		if(temp->next == NULL)
			break;

		temp = temp->next;
	}
}

void printPoolProc()
{
	list *temp = rootNode;
	int i=0;
	while(temp != NULL)
	{
		printf("[%d] %s %d\n", i, temp->pname, temp->pid);
		temp = temp->next;
		i++;
	}
}