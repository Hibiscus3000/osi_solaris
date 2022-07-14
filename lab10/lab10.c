
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	int cpid = fork();
	int wstatus;
	if (-1 == cpid)
	{
		perror("Wasn't able to create child process");
		return 1;
	}
	if (!cpid)
	{
		execvp(argv[1],&(argv[1]));
		perror("Wasn't able to execute program");
	}
	else
	{
		waitpid(cpid,&wstatus,0);
		if (!WIFEXITED(wstatus))
		{
			fprintf(stderr,"Child process with pid [%d] wasn`t terminated normally\n",cpid);
			return 1;
		}
		printf("Exit status of the child process: %d\n",WEXITSTATUS(wstatus));
	}
	return 0;
}

