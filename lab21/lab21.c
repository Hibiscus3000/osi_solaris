#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int count = 0;
struct termios savetty;

void sigintHandler(int sig);
void sigquitHandler(int sig);

int main()
{
	struct termios tty;
	if (-1 == tcgetattr(STDIN_FILENO,&tty))
	{
		perror("Wasn't able to get terminal attributes");
		return 1;
	}
	savetty = tty;
	setbuf(stdout,(char *)NULL);
	tty.c_lflag &= ~ECHO;
	tty.c_iflag &= ~IXON;
	if (-1 == tcsetattr(STDIN_FILENO,TCSAFLUSH,&tty))
	{
		perror("Wasn't able to set terminal attributes");
		return 1;
	}
	sigset(SIGINT,sigintHandler);
	sigset(SIGQUIT,sigquitHandler);
	while(1);
	return 0;
}

void sigintHandler(int sig)
{
	putchar(7);
	++count;
}

void sigquitHandler(int sig)
{
	printf("peep count: %d\n",count);
	if (-1 == tcsetattr(STDIN_FILENO,TCSAFLUSH,&savetty))
	{
		perror("Wasn't able to restore terminal attributes");
		exit(1);
	}
	exit(0);
}
