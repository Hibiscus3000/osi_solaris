#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

void erase(int * numberOfCharactersRead)
{
        putchar(8);
        putchar(' ');
        putchar(8);
        --(*numberOfCharactersRead);
}

void kill(int* numberOfCharactersRead)
{
	int i;
	for (i = 0; i < *numberOfCharactersRead; ++i)
	{
		putchar(8);
        	putchar(' ');
 	        putchar(8);
	}
	*numberOfCharactersRead = 0;
}

void werase(int * numberOfCharactersRead, int firstLetterIndex)
{
	int i;
	for (i = 0; i < *numberOfCharactersRead - firstLetterIndex; ++i)
	{
		putchar(8);
                putchar(' ');
                putchar(8);
	}
	*numberOfCharactersRead = firstLetterIndex;
}

int nonPrintCharTreat(char c_cc[], char c)
{
	int i;
	for (i = 0; i < NCCS; ++i)
		if (c == c_cc[i])
		{
			putchar(7);
			putchar('^');
			putchar('G');
			return 1;
		}
	return 0;
}

int main()
{
	struct termios tty,savetty;
	int fdin = fileno(stdin);
	if (-1 == fdin)
	{
		perror("Wasn't able to get stdin file descriptor");
		return 1;
	}
	if (-1 == tcgetattr(fdin,&tty))
	{
		perror("Wasn't able to get terminal attributes");
		return 1;
	}
	setbuf(stdout,(char*)NULL);
	savetty = tty;
	tty.c_lflag &= ECHOCTL & ~ECHO & ~ICANON;
	tty.c_iflag &= ~IXON;
	tty.c_cc[VMIN] = 1;
	if (-1 == tcsetattr(fdin,TCSAFLUSH,&tty))
        {
                perror("Wasn't able to set terminal attributes");
                return 1;
        }
	char c;
	int NPCheckRes;
	int numberOfCharactersRead = 0;
	int wordsFirstLetterIndex[20];
	int numberOfWords = 0;
	wordsFirstLetterIndex[0] = 0;
	int prevCharIsSpace = 1;
	while (1)
	{
		if (-1 == read(fdin,&c,1))
		{
			perror("\nWasn't able to read character");
			break;
		}
		if (CEOT == c)
			if (!numberOfCharactersRead)
				break;
			else
				continue;
		if (c == tty.c_cc[VERASE])
		{
			if (numberOfCharactersRead)
			{
				erase(&numberOfCharactersRead);
				if ((numberOfWords) && (wordsFirstLetterIndex[numberOfWords - 1] == numberOfCharactersRead))
					--numberOfWords;
			}
			continue;
		}
		if (c == tty.c_cc[VKILL])
		{
			if (numberOfCharactersRead)
			{
				kill(&numberOfCharactersRead);
				numberOfWords = 0;
			}
			continue;
		}
		if (tty.c_cc[VWERASE] == c)
		{
			if (numberOfCharactersRead)
				werase(&numberOfCharactersRead,wordsFirstLetterIndex[numberOfWords - 1]);
			if (numberOfWords)
				--numberOfWords;
			prevCharIsSpace = 1;
			continue;
		}
                if ((40 == numberOfCharactersRead) || ('\n' == c))
                {
                        numberOfCharactersRead = 0;
                        numberOfWords = 0;
                        wordsFirstLetterIndex[0] = 0;
			putchar('\n');
			if ('\n' == c)
 	        		continue;
                }
		NPCheckRes = nonPrintCharTreat(tty.c_cc,c);
		if ((!NPCheckRes) && (EOF == putchar(c)))
		{
			perror("\nWasn't able to write character");
			break;
		}
		if ((prevCharIsSpace) && (' ' != c))
                {
                        wordsFirstLetterIndex[numberOfWords] = numberOfCharactersRead;
                        ++numberOfWords;
                }
                prevCharIsSpace = (' ' == c);
		if (NPCheckRes)
			++numberOfCharactersRead;
		++numberOfCharactersRead;
	}
	if (-1 == tcsetattr(fdin,TCSAFLUSH,&savetty))
		perror("Wasn't able to return default terminal attributes");
	return 0;
}
