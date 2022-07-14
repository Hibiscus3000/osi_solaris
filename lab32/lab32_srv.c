#include <aio.h>
#include <errno.h>
#include <sys/socket.h>
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int numberOfClients;
int numberOfChars = 0;
int numberOfFinishedClients = 0;
int numberOfAcCon;
int s;
int *ns;
struct aiocb * list;

void initializeAsynchRead(struct aiocb * node);
void runServer(int signo, siginfo_t *info, void *context);
void closeSockets();
void freeReqList();

void initializeAsynchRead(struct aiocb * node)
{
	if (-1 == aio_read(node))
	{
		perror("aio_read() failed");
		close(s);
		freeReqList();
		closeSockets();
		free(ns);
		exit(1);
	}
}

void runServer(int signo, siginfo_t *info, void *context)
{
	if (SIGIO != signo || SIGIO != info->si_signo)
		return;
	int rc,wc;
	struct aiocb * node = (struct aiocb *)info->si_value.sival_ptr;
	rc = aio_return(node);
	if (0 == rc)
		++numberOfFinishedClients;
	else if (0 > rc)
	{
		perror("aio_return() returned < 0 argument: aio_read() failed");
		close(s);
		freeReqList();
		closeSockets();
		free(ns);
		exit(1);
	}
	else
		initializeAsynchRead(node);
	numberOfChars += rc;
	for (int j = 0; j < rc; ++j)
		printf("%c",toupper(((unsigned char*)node->aio_buf)[j]));
	if (numberOfFinishedClients == numberOfClients)
	{
		printf("\n\n[server] I have toupper()ed %d chars from %d clients\n",numberOfChars,numberOfClients);
		close(s);
		freeReqList();
		closeSockets();
		free(ns);
		exit(0);
	}
}

void closeSockets()
{
	if (!ns)
		return;
	for (int i = 0; i < numberOfAcCon; ++i)
		close(ns[i]);
}

void freeReqList()
{
	for (int i = 0; i < numberOfAcCon; ++i)
	{
		if (!(list[numberOfAcCon].aio_buf))
			continue;
		free(list[numberOfAcCon].aio_buf);
	}
	free(list);
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		fprintf(stderr,"[server] wrong number of arguments, received: %d, should be 3\n",argc);
		return 1;
	}
	struct sockaddr_un addr;

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

	numberOfClients = atoi(argv[2]);
	if (0 >= numberOfClients)
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


	numberOfChars = 0;
	ns = (int *)malloc(numberOfClients * sizeof(int));
	if (!ns)
	{
		fprintf(stderr,"wasn't able to allocate memory for new sockets");
		close(s);
		return -2;
	}
	list = (struct aiocb*)malloc(sizeof(struct aiocb)*numberOfClients);
	if (!list)
	{
		fprintf(stderr,"wasn't able to allocate memory for requests list");
		close(s);
		free(ns);
		return -2;
	}

	struct sigaction sigioHandlerAction;
	memset(&sigioHandlerAction,0,sizeof(sigioHandlerAction));
	sigioHandlerAction.sa_sigaction = runServer;
	sigioHandlerAction.sa_flags = SA_SIGINFO;
	sigaction(SIGIO,&sigioHandlerAction,NULL);

	numberOfAcCon = 0;
	while (numberOfAcCon < numberOfClients)
	{
		while (-1 == (ns[numberOfAcCon] = accept(s,NULL,NULL)));
		memset(&list[numberOfAcCon],0,sizeof(list[numberOfAcCon]));
		list[numberOfAcCon].aio_fildes = ns[numberOfAcCon];
		if (!(list[numberOfAcCon].aio_buf = (unsigned char *)malloc(sizeof(unsigned char) * BUFSIZ)))
		{
			closeSockets(ns, numberOfAcCon + 1);
			freeReqList(list, numberOfAcCon + 1);
			close(s);
			free(ns);
			fprintf(stderr,"wasn't able to allocate memory for next aiocb structure receive buffer");
			return -2;
		}
		list[numberOfAcCon].aio_nbytes = BUFSIZ;
		list[numberOfAcCon].aio_sigevent.sigev_signo = SIGIO;
		list[numberOfAcCon].aio_sigevent.sigev_notify = SIGEV_SIGNAL;
		list[numberOfAcCon].aio_sigevent.sigev_value.sival_ptr = &list[numberOfAcCon];
		initializeAsynchRead(&list[numberOfAcCon]);
		++numberOfAcCon;
	}
	while (1);
	return 0;
}
