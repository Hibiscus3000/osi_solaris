
#define READ_LINE_NUMBER_SECONDS 5

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

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
	if (-1 == lseek(fd, 0, SEEK_SET))
		perror("Wasn't able to return to the begining of the file");
	buf = (char*)malloc(fileLength + 1);
	if (!buf)
		fprintf(stderr,
			"Wasn't able to highlight memory for file`s content\n");
	buf[fileLength] = 0;
	if (-1 == read(fd, buf, fileLength))
		perror("Wasn't able to read from file");
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
	char* str;
	while (1)
	{
		if (!lineNumber)
			break;
		if (-1 == lseek(fd, offsetTable[lineNumber -1].offset, SEEK_SET))
		{
			perror("Wasn't able to reposition file offset");
			return;
		}
		str = (char*)malloc(offsetTable[lineNumber - 1].length + 1);
		if (!str)
		{
			fprintf(stderr,
				"Wasn't able to highlight memory for the line\n");
			return;
		}
		str[offsetTable[lineNumber - 1].length] = 0;
		if (-1 == read(fd, str, offsetTable[lineNumber - 1].length))
		{
			perror("Wasn't able to read the line");
			free(str);
			return;
		}
		if (-1 == write(1, str, offsetTable[lineNumber - 1].length))
		{
			perror("Wasn't able to write the line");
			free(str);
			return;
		}
		if (lineNumber == numberOfLines)
			printf("\n");
		free(str);
		readLineNumber(&lineNumber,numberOfLines);
	}
}

void printAll(int signal)
{
	printf("\n");
	if (-1 == write(1,buf,fileLength))
	{
		perror("Wasn't able to print all text");
		free(buf);
		free(offsetTable);
		exit(1);
	}
	printf("\n\n");
	free(buf);
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
		if (buf)
			free(buf);
		return 1;
	}
	printLines(fd,numberOfLines);
	if (-1 == close(fd))
	{
		perror("Wasn't able to close the file");
		free(offsetTable);
		if (buf)
			free(buf);
		return 1;
	}
	free(offsetTable);
	if (buf)
		free(buf);
	return 0;
}
