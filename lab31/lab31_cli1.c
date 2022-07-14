
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int s;

void catcher(int sig)
{
	close(s);
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		fprintf(stderr,"[client] wrong number of argumets\n");
		return 1;
	}

	signal(SIGPIPE,catcher);

	int n;
	if (0 >= (n = atoi(argv[3])))
	{
		fprintf(stderr,"wrong number of lines to be written to server argument.\nIt should be a number not less then one");
		return 1;
	}
	struct sockaddr_un addr;
	int rc,wc;
	if (-1 == (s = socket(AF_UNIX,SOCK_STREAM,0)))
	{
		perror("socket() failed");
		return -1;
	}

	memset(&addr,0,sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path,argv[1],sizeof(addr.sun_path) - 1);
	if (-1 == connect(s,(struct sockaddr*)&addr,sizeof(addr)))
	{
		perror("connect() failed");
		close(s);
		return 1;
	}
	for (int i = 0; i < n; ++i)
		if ((wc = write(s,argv[2],strlen(argv[2]))) != strlen(argv[2]))
		{
			if (wc > 0)
				fprintf(stderr,"partial write(), [rc] = %d, but [wc] = %d",rc,wc);
			else
				perror("write() failed");
			close(s);
			return 2;
		}
	close(s);
	return 0;
}
