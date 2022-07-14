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
	if (-1 == p2open("sort",fd))
	{
		perror("wasn't to perform \"p2open()\"");
		return 1;
	}
	for (i = 0; i < N; ++i)
	{
		num = (int)(((double)rand() / RAND_MAX) * (MAX - MIN)) + MIN;
		if (1 > fwrite(&num,sizeof(int),1,fd[0]))
		{
			perror("wasn't able to write() next number to \"sort\"");
			if (-1 == p2close(fd))
                		perror("wasn't able to perform \"p2close()\"");
                	return 1;
		}
		if ((EOF == fputc('\r',fd[0])) || (EOF == fputc('\n',fd[0])))
		{
			perror("wasn't able to fputc() carriage return symbol and a newline");
			if (-1 == p2close(fd))
				perror("wasn't able to perform \"p2close()\"");
			return 1;
		}
	}
	if (EOF == fclose(fd[0]))
	{
		perror("wasn't able to perform \"fclose()\" to file descriptor opened for writing in \"sort\"");
		if (EOF == fclose(fd[1]))
			perror("wasn't able to perform \"fclose()\" to file descriptor opened for reading from \"sort\"");
		return 1;
	}
	for (i = 0; i < N; ++i)
	{
		if (1 > fread(&num,sizeof(int),1,fd[1]))
		{
			perror("wasn't able to read() from \"sort\"");
			if (EOF == fclose(fd[1]))
                		 perror("wasn't able to perform \"fclose()\" to file descriptor opened for reading from \"sort\"");
                	return 1;
		}
		printf("%d ",num);
		if (!((i + 1) % 10))
			printf("\n");
		if ((EOF == fgetc(fd[1])) || (EOF == fgetc(fd[1])))
		{
			perror("fgetc() failed");
			if (EOF == fclose(fd[1]))
                        	perror("wasn't able to perform \"fclose()\" to file descriptor opened for reading from \"sort\"");
                        return 1;
		}
	}
	if (EOF == fclose(fd[1]))
	{
		perror("wasn't able to perform \"fclose()\" to file descriptor opened for reading from \"sort\"");
		return 1;
	}
	return 0;
}
