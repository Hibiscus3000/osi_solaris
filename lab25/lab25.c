
#define N 20

#include <ctype.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int fd[2];
	if (-1 == pipe(fd))
	{
		perror("Wasn't able to create program channel");
		return 1;
	}
	pid_t wpid;
	if (!(wpid = fork()))
	{
		unsigned char buf[N+1];
		int numberOfReadChars;
		if (-1 == (numberOfReadChars = read(0,buf,N)))
		{
			perror("Wasn't able to read line from terminal");
			close(fd[0]);
			close(fd[1]);
			return 1;
		}
		buf[numberOfReadChars] = 0;
		if (-1 == write(fd[0],buf,numberOfReadChars))
		{
			perror("Wasn't able to write line to program channel");
			close(fd[0]);
			close(fd[1]);
			return 1;
		}
	}
	else if (wpid > 0)
	{
		int wstatus;
		waitpid(wpid,&wstatus,0);
		if (!WIFEXITED(wstatus))
		{
			perror("Writing process wasn't terminated normally");
			close(fd[0]);
			return 1;
		}
		int rpid;
		if (!(rpid = fork()))
		{
			unsigned char buf[N+1];
			int numberOfReadChars;
			if (-1 == (numberOfReadChars = read(fd[1],buf,N)))
			{
				perror("Wasn't able to read line from program channel");
                        	close(fd[0]);
				close(fd[1]);
       	                	return 1;
			}					
			buf[numberOfReadChars] = 0;
			int i;
			for (i = 0; i < numberOfReadChars; ++i)
				buf[i] = toupper(buf[i]);
			if (-1 == write(1,buf,numberOfReadChars))
       	        	{
                       		perror("Wasn't able to write uppercase line to terminal");
                       		close(fd[0]);
				close(fd[1]);
                       		return 1;
                	}
		}
		else if (rpid > 0)
		{
			waitpid(rpid,&wstatus,0);
			if (!WIFEXITED(wstatus))
                	{
                        	perror("Reading process wasn't terminated normally");
                        	close(fd[0]);
				close(fd[1]);
                        	return 1;
                	}

		}
		else
		{
			perror("Wasn't able to perform second fork");
                	close(fd[0]);
			close(fd[1]);
                	return 1;
		}
	}
	else
	{
		perror("Wasn't able to perform first fork");
		close(fd[1]);
		close(fd[0]);
		return 1;
	}
	close(fd[0]);
	close(fd[1]);
	return 0;
}
