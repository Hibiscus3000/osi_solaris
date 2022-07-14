
#include <stdio.h>
#include <unistd.h>

extern char** environ;

int execvpe(const char* file, char* const argv[], char* const envp[])
{
	environ = envp;
	return execvp(file,argv);
}

void check(char* argv[])
{
	char* nargv[] = {argv[1],(char*)NULL};
	execvpe(argv[1],nargv,&argv[2]);
	perror("Wasn't able to perform execvpe");
}

int main(int argc, char* argv[], char** envp[])
{
	check(argv);
	return 0;
}
