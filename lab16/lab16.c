
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int main()
{
	int fdin = fileno(stdin);
	if (-1 == fdin)
	{
		perror("Wasn't able to get stdin file descriptor");
		return -1;
	}
	
	if (!isatty(fileno(stdout)))
	{
		perror("stdout file descriptor isn't referring to a terminal");
		return 1;
	}

	if (setvbuf(stdout,(char *)NULL,_IONBF,BUFSIZ))
	{
		perror("Wasn't able to make stdout unbuffered");
		return 1;
	}
	struct termios tty,savetty;
	if (-1 == tcgetattr(fdin,&tty))
	{
		perror("Wasn't able to get terminal attributes");
		return -1;
	}
	savetty = tty;

	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 0;
	tty.c_lflag &= ~ICANON;
	if (-1 == tcsetattr(fdin,TCSANOW,&tty))
	{
		perror("Wasn't able to set terminal attributes");
		return -1;
	}

	char numeral;
        printf("Write a numeral: ");
	if (-1 == read(fdin,&numeral,1))
		perror("\nWasn't able to read numeral");
	else
	{
		if ((numeral - '0' >= 0) && (numeral - '0' <= 9))
			printf("\nYou wrote \"%c\"\n",numeral);
		else
			printf("\n\"%c\" is not a numeral\n",numeral);
	}

	if (-1 == tcsetattr(fdin,TCSANOW,&savetty))
	{
		perror("Wasn't able to return previous terminal attributes");
		return -1;
	}
	
	printf("Write a numeral and an endline: ");
	if (-1 == read(fdin,&numeral,1))
                perror("Wasn't able to read numeral");
        else
        {
                if ((numeral - '0' >= 0) && (numeral - '0' <= 9))
                        printf("You wrote \"%c\"\n",numeral);
                else
                        printf("\"%c\" is not a numeral\n",numeral);
        }
	return 0;
}
