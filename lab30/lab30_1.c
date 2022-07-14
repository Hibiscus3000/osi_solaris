
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <tiuser.h>
#include <unistd.h>

#define SRV_ADDR 1
#define BUFSIZE 1024

int conn_fd;

void run_server(int listen_fd)
{
	char c;

	switch(fork())
	{
		case -1:
		{
			perror("fork() failed");
			exit(-1);
		}
		case 0:
		{
			if (0 > t_close(conn_fd))
			{
				t_error("t_close() failed - conn_fd");
				exit(7);
			}
		}
		default:
		{
			if (0 > t_close(listen_fd))
			{
				t_error("t_close() failed - listen_fd");
				exit(7);
			}
			char buf[BUFSIZE];
			int recvcount;
			if (0 > (recvcount = t_rcv(conn_fd,buf,BUFSIZE,0)))
			{
				t_error("t_recv() failed");
				exit(11);
			}
			if (0 > t_sndrel(conn_fd))
                        {
                                 t_error("t_sndrel() failed");
                                 exit(10);
                     	}
			int i;
			for (i = 0; i < recvcount; ++i)
				buf[i] = toupper(buf[i]);
			if (1 > write(0,buf,BUFSIZE - 1))
			{
				perror("write() failed");
				exit(12);
			}
		}
	}
}

int main()
{
	pid_t cpid = fork();
	switch (cpid)
	{
		case 0: //client
		{
			int fd;
			struct t_call *sndcall;
			if (0 > (fd = t_open("/dev/tivc",O_RDWR,NULL)))
			{
				t_error("t_open() failed");
				return 1;
			}

			if (0 > t_bind(fd,NULL,NULL))
			{
				t_error("t_bind() failed");
				return 2;
			}

			if (!(sndcall = (struct t_call*)t_alloc(fd,T_CALL,T_ADDR)))
			{
				t_error("t_alloc() of t_call failed");
				return 3;
			}
			sndcall->addr.len = sizeof(int);
			*(int *)sndcall->addr.buf = SRV_ADDR;

			if (0 > t_connect(fd, sndcall, NULL))
			{
				t_error("t_connect() failed");
				return 5;
			}

			char buf[BUFSIZE];
			buf[BUFSIZE - 1] = 0;
			if (1 > read(1,buf,BUFSIZE - 1))
			{
				perror("wasn't able to read() input text");
				return 8;
			}
			if (0 > t_snd(fd,buf,BUFSIZE,0))
			{
				t_error("t_snd() failed");
				return 9;
			}
			if (0 > t_sndrel(fd))
			{
				t_error("t_sndrel() failed");
				return 10;
			}
			break;
		}
		case -1:
		{
			perror("fork() failed");
			return -1;
		}
		default: //server
		{
			int listen_fd;
			struct t_bind *bind;
			struct t_call *call;
			if (0 > (listen_fd = t_open("/dev/tivc",O_RDWR,NULL)))
			{
				t_error("t_open() failed");
				return 1;
			}

			if (!(bind = (struct t_bind*)t_alloc(listen_fd,T_BIND,T_ALL)))
			{
				t_error("t_alloc() of t_bin failed");
				return 3;
			}
			bind->qlen = 1;
			bind->addr.len = sizeof(int);
			*(int *)bind->addr.buf = SRV_ADDR;
			if (0 > t_bind(listen_fd, bind, bind))
			{
				t_error("t_bind() failed");
				return 2;
			}

			if (SRV_ADDR != *(int *)bind->addr.buf)
			{
				fprintf(stderr,"server got wrong address during binding");
				return 4;
			}

			if (!(call = (struct t_call *)t_alloc(listen_fd,T_CALL,T_ALL)))
			{
				t_error("t_alloc() of t_call failed");
				return 3;
			}

			while (1)
			{
				if (0 > t_listen(listen_fd,call))
				{
					t_error("t_listen() failed");
					return 6;
				}

				if (T_DISCONNECT != (conn_fd = t_accept(listen_fd,listen_fd,call)))
					run_server(listen_fd);
			}
		}
	}
	return 0;
}

