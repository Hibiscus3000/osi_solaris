#define WORKING_DIRECTORY_MAX_NAME_LENGTH 100

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <errno.h>

void i()
{
	printf("Real user ID: %d.\nEffective user ID: %d.\n",getuid(), geteuid());
        printf("Real group ID: %d.\nEffective group ID: %d.\n",getgid(), getegid());
}

void s(int* sflg)
{
    if (!*sflg)
    {	
	if (!setpgid(0,0))
	{
	    printf("Calling process was made a group leader.\n");
	    ++(*sflg);
	}
	else
	    printf("Wasn't able to make calling process a qroup leader.\n");
    }
    else
	printf("Calling process is already a group leader.\n");
}

void p()
{
    printf("Process ID: %d.\nProcess-parent ID: %d.\nGroup ID: %d.\n",getpid(),getppid(),getpgid(getpid()));
}

void u()
{
    struct rlimit rlim;
    if (-1 == getrlimit(RLIMIT_FSIZE,&rlim))
	printf("Couldn't find ulimit value.\n");
    printf("Ulimit soft: %lu.\n", rlim.rlim_cur);
    printf("Ulimit hard: %lu.\n", rlim.rlim_max);
}

void U(char* optarg)
{
    struct rlimit rlim;
    char * end_ptr;
    int new_ulimit = strtol(optarg, &end_ptr, 10);
    if ((end_ptr == optarg) || (new_ulimit < 0))
	printf("Bad new ulimit value given: %s.\n",optarg);
    else
    {
	rlim.rlim_max = new_ulimit;
	rlim.rlim_cur = new_ulimit;
	if (-1 == setrlimit(RLIMIT_FSIZE, &rlim))
	    printf("Coudld't change ulimit to %d.\n",new_ulimit);
	else
	    printf("Ulimit changed to %d.\n",new_ulimit);
    }
}

void c()
{
    struct rlimit rlim;	
    if (-1 == getrlimit(RLIMIT_CORE,&rlim))
	printf("Couldn't find core size.\n");
    printf("Core size sotf: %d.\n",rlim.rlim_cur);
    printf("Core size hard: %d.\n",rlim.rlim_max);
}

void C(char* optarg)
{
    struct rlimit rlim;
    char* end_ptr;
    int new_core_size = strtol(optarg, &end_ptr,10);
    if ((end_ptr == optarg) || (new_core_size < 0))
	printf("Bad new core size given: %s.\n",optarg);
    else
    {
	rlim.rlim_cur = new_core_size;
	rlim.rlim_max = new_core_size;
	if(-1 == setrlimit(RLIMIT_CORE, &rlim))
	    printf("Couldn't change core size to %d.\n",new_core_size);
	else
	    printf("Core size changed to %d.\n", new_core_size);    
    }
}

void d()
{
    char* working_directory_name = (char*)malloc(WORKING_DIRECTORY_MAX_NAME_LENGTH);
    getcwd(working_directory_name,WORKING_DIRECTORY_MAX_NAME_LENGTH);
    if (ERANGE == errno)
	printf("Current working directory name is too long.\n");
    else
	printf("Current working directory name: %s\n",working_directory_name);
}

void v(char ** envp)
{
    char ** p;
    printf("Environment variables are:\n");
    for (p = envp;*p;++p)
	printf("%s\n", *p);
}

void V(char* optarg)
{
    if (!putenv(optarg))
	printf("The variable %s was added to an environment.\n",optarg);
    else
	printf("Couldn't add %s to the evironment.",optarg);
}

void unrecognized_option(int optopt, int* invalid)
{
    printf("Invalid option is %c.\n",optopt);
    ++(*invalid);
}

int main(int argc, char * argv[], char **envp)
{
    char options[] = "ispuU:cC:dvV:";
    int o, invalid = 0, sflg = 0;
    printf("argc equals %d.\n", argc);
    while ((o = getopt(argc,argv,options)) != EOF)
    {
	switch (o)
	{
		case 'i':
			i();
			break;
		case 's':
			s(&sflg);
			break;
		case 'p':
			p();
			break;
		case 'u':
			u();
			break;
		case 'U':
	    		U(optarg);
			break;
		case 'c':
			c();
			break;
		case 'C':
			C(optarg);
			break;
		case 'd':
			d();
			break;
		case 'v':
			v(envp);
			break;
		case 'V':
			V(optarg);
			break;
		case '?':
			unrecognized_option(optopt,&invalid);
	}	
    }
    printf("Number of invalid options: %d.\n", invalid);
    return 0;
}
