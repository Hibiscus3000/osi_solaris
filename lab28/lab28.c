#define N 100
#define MIN 0
#define MAX 99

#include <unistd.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	srand(time(NULL));
	int num;
	int i;

	FILE * fd[2];
	if (-1 == p2open("sort -n",fd))
	{
		perror("wasn't to perform \"p2open()\"");
		return 1;
	}
	for (i = 0; i < N; ++i)
	{
		num = (int)(((double)rand() / RAND_MAX) * (MAX - MIN)) + MIN;
		fprintf(fd[0],"%d\n",num);
	}
	if (EOF == fclose(fd[0]))
	{
		perror("wasn't able to perform \"fclose()\" to file descriptor opened for writing in \"sort\"");
		if (EOF == fclose(fd[1]))
			perror("wasn't able to perform \"fclose()\" to file descriptor opened for reading from \"sort\"");
		return 1;
	}
	char c;
	for (i = 0; i < N; ++i)
	{
		fscanf(fd[1],"%d",&num);
		printf("%d ",num);
		if (!((i + 1) % 10))
			printf("\n");
	}
	if (EOF == fclose(fd[1]))
	{
		perror("wasn't able to perform \"fclose()\" to file descriptor opened for reading from \"sort\"");
		return 1;
	}
	return 0;
}
