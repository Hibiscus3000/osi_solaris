
#include <stdio.h>

extern char** environ;

int main(int argc,char* argv[],char* envp[])
{
	printf("My args are:\n");
	char** p;
	for (p = argv; *p != NULL; ++p)
		printf("	%s\n",*p);
	printf("\nMy env variables are:\n");
	for (p = environ; *p != NULL; ++p)
		printf("	%s\n",*p);
	return 0;
}
