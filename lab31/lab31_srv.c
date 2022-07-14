#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

unsigned char runServer(int* numberOfChars, struct pollfd* pollns,int* ns, int numberOfAcCon, int numberOfCli, int* numberOfFinishedCli,
int* clientIsFinished);
void closeSockets(int* ns, int numberOfAcCon);

unsigned char runServer(int* numberOfChars, struct pollfd* pollns,int* ns, int numberOfAcCon, int numberOfCli,int* numberOfFinishedCli,
int* clientIsFinished)
{
	char buf[BUFSIZ];
	buf[BUFSIZ - 1] = 0;
	int rc,wc;
	for (int i = 0; i < numberOfAcCon; ++i)
		memset(&(pollns[i].revents),0,sizeof(pollns[i].revents));
	int count = poll(pollns,numberOfAcCon,-1);
	if (-1 == count)
	{
		perror("poll() failed");
		return 1;
	}
	if (count > 0)
	{
		for (int i = 0; i < numberOfAcCon; ++i)
		{
			if (!(clientIsFinished[i]) && (pollns[i].revents & POLLIN))
			{
				rc = read(ns[i],buf,sizeof(buf) - 1);
				if (-1 == rc)
				{
					perror("read() failed");
					return 1;
				}
				if ((0 == rc) && (pollns[i].revents & POLLHUP))
				{
					++(*numberOfFinishedCli);
					if (*numberOfFinishedCli == numberOfCli)
						return 0;
					clientIsFinished[i] = 1;
					continue;
				}
				(*numberOfChars) += rc;
				for (int j = 0; j < rc; ++j)
					buf[j] = toupper(buf[j]);
				wc = write(STDOUT_FILENO,buf,rc);
				if (rc > wc)
				{
					if (wc < 0)
						perror("write() failed");
					else
						fprintf(stderr,"partial write");
					return 1;
				}
			}
		}
	}
	return 2;
}

void closeSockets(int* ns, int numberOfAcCon)
{
	if (!ns)
		return;
	for (int i = 0; i < numberOfAcCon; ++i)
		close(ns[i]);
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		fprintf(stderr,"[server] wrong number of arguments, received: %d, should be 3\n",argc);
		return 1;
	}
	struct sockaddr_un addr;

	int s;

	if (-1 == (s = socket(AF_UNIX,SOCK_STREAM,0)))
	{
		perror("socket() failed");
		return -1;
	}

	memset(&addr,0,sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path,argv[1],sizeof(addr.sun_path) - 1);
	unlink(addr.sun_path);

	if (-1 == bind(s,(struct sockaddr *)&addr,sizeof(addr)))
	{
		perror("bind() failed");
		close(s);
		return -1;
	}

	if (-1 == listen(s,1))
	{
		perror("listen() failed");
		close(s);
		return -1;
	}

	int numberOfCli = atoi(argv[2]);
	if (0 >= numberOfCli)
	{
		fprintf(stderr,"wrong number of clients argument");
		close(s);
		return 1;
	}

	if (-1 == accept(s,NULL,NULL))
	{
		close(s);
		perror("test accept() failed");
		return -1;
	}

	int numberOfChars = 0;
	int *ns = malloc(sizeof(int) * numberOfCli);
	struct pollfd* pollns = malloc(sizeof(struct pollfd) * numberOfCli);
	int numberOfAcCon = 0;
	int numberOfFinishedCli = 0;
	int* clientIsFinished = (int*)calloc(numberOfCli,sizeof(int));
	while (numberOfAcCon < numberOfCli)
	{
		if (-1 == (ns[numberOfAcCon] = accept(s,NULL,NULL)))
		{
			closeSockets(ns,numberOfAcCon);
			close(s);
			free(ns);
			free(pollns);
			perror("accept() failed");
			return -1;
		}
		else
		{
			pollns[numberOfAcCon].events = POLLIN;
			pollns[numberOfAcCon].fd = ns[numberOfAcCon];
			++numberOfAcCon;
		}
		if (numberOfAcCon == numberOfCli)
			break;
		if (1 == runServer(&numberOfChars,pollns,ns,numberOfAcCon, numberOfCli,&numberOfFinishedCli,clientIsFinished))
		{
			closeSockets(ns,numberOfAcCon);
			free(ns);
			free(pollns);
			close(s);
			return -1;
		}
	}
	unsigned char runServerResult;
	while (2 == (runServerResult = runServer(&numberOfChars, pollns,ns,numberOfAcCon, numberOfCli,&numberOfFinishedCli,clientIsFinished)));
	printf("\n\n[server] I have toupper()ed %d chars from %d clients",numberOfChars,numberOfCli);
	closeSockets(ns,numberOfAcCon);
	free(ns);
	free(pollns);
	close(s);
	return (int)runServerResult;
}
