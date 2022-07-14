#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	printf("User real ID: %d\nUser effective ID: %d\n",getuid(),geteuid());
	FILE* file;
	if (file = fopen("./file","r+"))
	{
		if (!fclose(file))
			printf("File was successfully opened and closed\n");
		else
			perror("Wasn't able to close the file");
	}
	else
		perror("Wasn't able to open the file");

	if (setuid(getuid()))
		perror("\nWasn't able to change user effective ID ");
	else
		printf("User effective ID was set equil to user real ID\n");

	printf("User real ID: %d\nUser effective ID: %d\n",getuid(),geteuid());
        if (file = fopen("./file","r+"))
        {
                if (!fclose(file))
                        printf("File was successfully opened and closed\n");
                else
                        perror("Wasn't able to close the file");
        }
        else
                perror("Wasn't able to open the file");

	return 0;
}
