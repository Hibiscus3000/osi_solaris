#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		fprintf(stderr,"wrong number of arguments\n");
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

	int ns;
	if (-1 == (ns = accept(s,NULL,NULL)))
	{
		close(s);
		perror("accept() failed");
		return -1;
	}
	close(s);
	char buf[BUFSIZ];
	buf[BUFSIZ - 1] = 0;
	int rc,wc;
	while ((rc = read(ns,buf,sizeof(buf) - 1)) > 0)
	{
		for (int i = 0; i < rc; ++i)
			buf[i] = toupper(buf[i]);
		wc = write(STDOUT_FILENO,buf,rc);
		if (rc > wc)
		{
			if (wc < 0)
				perror("write() failed");
			else
				fprintf(stderr,"partial write");
			close(s);
			close(ns);
			return 2;
		}
	}
	close(ns);
	if (-1 == rc)
	{
		perror("read() failed");
		return -1;
	}
	return 0;
}
