#define TIME_OUT 1

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int fileNumber;
int numberOfFiles;
int* eofs;

int openFiles(int argc, char* argv[], int** fds)
{
	*fds = (int*)malloc((argc - 1) * sizeof(int));
	eofs = (int*)malloc((argc - 1) * sizeof(int));
	int i, j;
	for (i = 1; i < argc; ++i)
	{
		(*fds)[i - 1] = open(argv[i], O_RDONLY);
		eofs[i - 1] = 0;
		if (-1 == (*fds)[i - 1])
		{
			perror("Wasn't able to open file");
			for (j = i - 2; j >= 0; --j)
				if (-1 == close((*fds)[j]))
					perror("Wasn't able to close file");
			free(*fds);
			free(eofs);
			return 1;
		}
	}
	return 0;
}

void closeFiles(int* fds)
{
	int i;
	for (i = 0; i < numberOfFiles; ++i)
		if (-1 == close(fds[i]))
			perror("Wasn't able to close file");
	free(eofs);
	free(fds);
}

void chooseNextFileNumber(int signal)
{
	int i;
	int currentFileNumber = fileNumber;
	for (i = currentFileNumber + 1; i < numberOfFiles; ++i)
		if (!eofs[i])
		{
			fileNumber = i;
			return;
		}
	for (i = 0; i < currentFileNumber; ++i)
		if (!eofs[i])
		{
			fileNumber = i;
			return;
		}
}

void readLines(int* fds)
{
	char* buf = (char*)malloc(5*sizeof(char));
	fileNumber = 0;
	int charNumber = 0;
	int numberOfOpenFiles = numberOfFiles;
	int numberOfReadElements;
	while (numberOfOpenFiles)
	{
		alarm(TIME_OUT);
		numberOfReadElements = read(fds[fileNumber], buf + charNumber, 1);
		if (1 > numberOfReadElements)
		{
			eofs[fileNumber] = 1;
			--numberOfOpenFiles;
			--charNumber;
		}
		++charNumber;
		if ((5 == charNumber) || ('\n' == buf[charNumber - 1]) || (!numberOfReadElements))
		{
			buf[charNumber] = 0;
			write(1, buf, charNumber);
			charNumber = 0;
			if (('\n' == buf[charNumber - 1]) || (!numberOfReadElements))
			chooseNextFileNumber(0);
		}
	}
	free(buf);
}

int main(int argc, char* argv[])
{
	int* fds;
	signal(SIGALRM,chooseNextFileNumber);
	if (openFiles(argc, argv, &fds))
		return 1;
	numberOfFiles = argc - 1;
	readLines(fds);
	closeFiles(fds);
	return 0;
}


