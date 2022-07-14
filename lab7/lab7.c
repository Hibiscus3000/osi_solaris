
#define READ_LINE_NUMBER_SECONDS 5

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

char* buf;
int fileLength;

struct line
{
	int offset;
	int length;
};

struct line* offsetTable;

void fillOffsetTable()
{
	int lineNumber = 0;
	offsetTable[0].offset = 0;
	int i;
	for (i = 0; i < fileLength; ++i)
		if ('\n' == buf[i])
		{
			offsetTable[lineNumber].length = i -
				offsetTable[lineNumber].offset + 1;
			++lineNumber;
			offsetTable[lineNumber].offset = i + 1;
		}
	offsetTable[lineNumber].length = fileLength -
		offsetTable[lineNumber].offset;
}

void createOffsetTable(int fd, int* numberOfLines)
{
	fileLength = lseek(fd, 0, SEEK_END);
	if (-1 == fileLength)
		perror("Wasn't able to find file length");
	buf = (char*)mmap(NULL,fileLength,PROT_READ,MAP_SHARED,fd,0);
	if (MAP_FAILED == buf)
		perror("Wasn't able to highlight memory for file`s content");
	*numberOfLines = 1;
	int i;
	for (i = 0; i < fileLength; ++i)
		if ('\n' == buf[i])
			++(*numberOfLines);
	offsetTable = (struct line*)malloc(sizeof(struct line) * *numberOfLines);
	if (!offsetTable)
		fprintf(stderr,
			"Wasn't able to highlight memory for offset table\n");
	fillOffsetTable();
}

void readLineNumber(int* lineNumber, int numberOfLines)
{
	alarm(READ_LINE_NUMBER_SECONDS);
	*lineNumber = 0;
	while ((1 != scanf("%d",lineNumber)) || (0 > *lineNumber) || (numberOfLines < *lineNumber))
	{
		getchar();
		if (0 > *lineNumber)
			fprintf(stderr,"Line number can't be less then zero\n");
		else if (numberOfLines < *lineNumber)
			fprintf(stderr,"Line number couldn't exceed number of lines it given file, which is %d\n",numberOfLines);
		else
			fprintf(stderr,"Wasn't able to read line number\n");
		*lineNumber = 0;
	}
}

void printLines(int fd, int numberOfLines)
{
	int lineNumber;
	readLineNumber(&lineNumber,numberOfLines);
	while (1)
	{
		if (!lineNumber)
			break;
		if (-1 == write(1, buf + offsetTable[lineNumber - 1].offset, offsetTable[lineNumber - 1].length))
		{
			perror("Wasn't able to write the line");
			return;
		}
		if (lineNumber == numberOfLines)
			printf("\n");
		readLineNumber(&lineNumber,numberOfLines);
	}
}

void printAll(int signal)
{
	printf("\n");
	if (-1 == write(1,buf,fileLength))
	{
		perror("Wasn't able to print all text");
		munmap(buf,fileLength);
		free(offsetTable);
		exit(1);
	}
	printf("\n\n");
	munmap(buf,fileLength);
	free(offsetTable);
	exit(0);
}

int main(int argc, char** argv)
{
	int fd = open(argv[1], O_RDONLY);
	if (-1 == fd)
	{
		perror("Wasn't able to open file");
		return 1;
	}
	int numberOfLines;
	offsetTable = NULL;
	createOffsetTable(fd,&numberOfLines);
	signal(SIGALRM,printAll);
	if (!offsetTable)
	{
		if (-1 == close(fd))
			perror("\nWasn't able to close the file");
		munmap(buf,fileLength);
		return 1;
	}
	printLines(fd,numberOfLines);
	if (-1 == close(fd))
	{
		perror("Wasn't able to close the file");
		free(offsetTable);
		munmap(buf,fileLength);
		return 1;
	}
	free(offsetTable);
	munmap(buf,fileLength);
	return 0;
}
