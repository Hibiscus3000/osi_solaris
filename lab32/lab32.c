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

pid_t createClient(int i, char* socketName, char* numberOfLines)
{
	sleep(1);
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
			execlp("./lab32_cli.out","./lab32_cli.out",socketName,clientLines[i % NUMBER_OF_CLIENTS],numberOfLines,(char *)NULL);
			perror("execlp() failed");
			return -1;
		}
		default:
		{
			return cpid;
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
	pid_t srvPid = fork();
	switch (srvPid)
	{
		case -1:
		{
			perror("fork() failed");
			return -1;
		}
		case 0:
		{
			execlp("./lab32_srv.out","./lab32_srv.out",argv[1],argv[2],(char *)NULL);
			perror("execlp() failed");
			return 1;
		}
		default:
		{
			pid_t* clientPids = (pid_t*)malloc(sizeof(pid_t) * numOfCli);
			if (adjustServConnection(argv[1]))
				return 2;
			while (i < numOfCli)
			{
				if (-1 == (clientPids[i] = createClient(i,argv[1],argv[3])))
				{
					fprintf(stderr,"wasn't able to create child process");
					kill(0,SIGKILL);
					free(clientPids);
					return -2;
				}
				++i;
			}
			int wstatus;
			if (-1 == waitpid(srvPid,&wstatus,0))
			{
				perror("\nwaipid() failed");
				free(clientPids);
				return 3;
			}
			if (WIFEXITED(wstatus))
				printf("\n\nserver [%d] finished with code: %d\n",srvPid,WEXITSTATUS(wstatus));
			for (i = 0; i < numOfCli; ++i)
			{
				if (-1 == waitpid(clientPids[i],&wstatus,0))
				{
					perror("\nwaitpid() failed");
					continue;
				}
				if (WIFEXITED(wstatus))
					printf("\nclient [%d] finished with code: %d",clientPids[i],WEXITSTATUS(wstatus));
			}
			printf("\n");
			free(clientPids);
		}
	}
	return 0;
}
