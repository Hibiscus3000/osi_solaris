
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	pid_t cpid = fork();
	int wstatus;
	if (-1 == cpid)
	{
		perror("Wasn't able to create child process");
		return 1;
	}
	if (!cpid)
	{
		execl("/bin/cat","cat",argv[1],(char *)NULL);
		perror("Wasn't able to call cat");
		return 1; 
	}
	else
	{
		waitpid(cpid,&wstatus,0);
		if (!WIFEXITED(wstatus))
			fprintf(stderr,"Child process with pid [%d] wasn`t terminated normally\n",cpid);
		printf("\n%s\n",argv[2]);
	}
	return 0;
}
