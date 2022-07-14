#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char *tzname[];
extern char **environ;

int main()
{
	char PST[14] = "TZ=PST08:00";
	if (putenv(PST))
	{
		perror("Wasn't able to change time via putenv");
		return 1;
	}
	time_t now;
	time(&now);
	struct tm *sp;
	printf("%s",ctime(&now));
	sp = localtime(&now);
	printf("%d/%d/%02d %d:%02d:%02d %s\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year, sp->tm_hour,
        sp->tm_min,sp->tm_min,tzname[sp->tm_isdst]);
	return 0;
}
