#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	switch (fork())
	{
		case -1:
		{
			perror("fork() failed");
			return -1;
		}
		case 0:
		{
			execvp("./lab30_cli.out",argv);
			perror("execvp() failed");
			return 1;
		}
		default:
		{
			execvp("./lab30_srv.out",argv);
			perror("execvp() failed");
			return 1;
		}
	}
	return 0;
}
