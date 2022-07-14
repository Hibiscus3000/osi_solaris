
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct List
{
	char* str;
	struct List* next;
};

void writeLinesAndClearList(struct List* head)
{
	struct List* curNode;
	printf("\n");
	while (head)
	{
		if (-1 == write(1,head->str,strlen(head->str)))
			perror("Wasn't able to print line");
		curNode = head;
		head = head->next;
		free(curNode->str);
		free(curNode); 
	}
}

void initializeNode(struct List* prevNode,struct List** newNode, char* str,int length)
{
	*newNode = (struct List*)malloc(sizeof(struct List)*length);
	if (!*newNode)
	{
		fprintf(stderr,"Wasn't able to highlight memory for the next line\n");
		return;
	}
	(*newNode)->str = str;
	(*newNode)->next = NULL;
	if (prevNode)
		prevNode->next = *newNode;
}

void readLines()
{
	char * str;
	char* buf = (char*)malloc(sizeof(char)*_POSIX2_LINE_MAX);
	int bufLength;
	struct List* head = NULL;
	struct List* tail= NULL;
	struct List* newNode = NULL;
	while (buf)
	{
		if (!fgets(buf,_POSIX2_LINE_MAX,stdin))
		{
			perror("Wasn't able to read next line");
			break;
		}
		if ('.' == buf[0])
			break;
		bufLength = strlen(buf);
		str = (char *)malloc(sizeof(char)*(bufLength + 1));
		strncpy(str,buf,bufLength);
		str[bufLength] = 0;
		initializeNode(tail,&newNode,str,bufLength);
		if (!newNode)
		{
			free(str);
			break;
		}
		tail = newNode;
		if (!head)
			head = newNode;
	}
	writeLinesAndClearList(head);
	free(buf);
}

int main(int argc, char** argv)
{
	readLines();
	return 0;
}




