#define NUMBER_OF_CLIENTS 10

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <unistd.h>

const char * clientLines[] = {"zero","one","two","three","four","five","six","seven","eight","nine"};

unsigned char adjustServConnection(char * socketName)
{
	int s;
	struct sockaddr_un addr;
	if (-1 == (s = socket(AF_UNIX,SOCK_STREAM,0)))
	{
		perror("socket() failed");
		return -1;
	}
	memset(&addr,0,sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path,socketName,sizeof(addr.sun_path) - 1);
	sleep(2);
	while (-1 == connect(s,(struct sockaddr*)&addr,sizeof(addr)));
	return 0;
}

unsigned char createClient(int i, char* socketName, char* numberOfLines)
{
	sleep(1);
	switch (fork())
	{
		case -1:
		{
			perror("fork() failed");
			return -1;
		}
		case 0:
		{
			execlp("./lab31_cli1.out","./lab31_cli1.out",socketName,clientLines[i % NUMBER_OF_CLIENTS],numberOfLines,(char *)NULL);
			perror("execlp() failed");
			return 1;
		}
		default:
		{
			return 0;
		}
	}
}

int main(int argc, char* argv[])
{
	int i = 0;
	if (4 > argc)
	{
		fprintf(stderr,"Wrong number of arguments.\nShould be 3 arguments:\n\t1 - socket name\n\t2 - number of clients\n\t");
		fprintf(stderr,"3 - number of lines to be written to the server by every client\n");
		return 1;
	}
	int numOfCli = atoi(argv[2]);
	if (0 >= numOfCli)
	{
		fprintf(stderr,"Wrong number of clients");
		return 1;
	}
	pid_t cpid = fork();
	switch (cpid)
	{
		case -1:
		{
			perror("fork() failed");
			return -1;
		}
		case 0:
		{
			execlp("./lab31_srv.out","./lab31_srv.out",argv[1],argv[2],(char *)NULL);
			perror("execlp() failed");
			return 1;
		}
		default:
		{
			if (adjustServConnection(argv[1]))
				return 2;
			while (i < numOfCli)
			{
				if (createClient(i,argv[1],argv[3]))
				{
					fprintf(stderr,"wasn't able to create new client");
					kill(0,SIGKILL);
					return -2;
				}
				++i;
			}
			int wstatus;
			if (-1 == waitpid(cpid,&wstatus,0))
			{
				perror("\nwaipid() failed");
				return 3;
			}
			if (WIFEXITED(wstatus))
				printf("\nserver [%d] finished with code: %d\n",cpid,WEXITSTATUS(wstatus));
		}
	}
	return 0;
}
