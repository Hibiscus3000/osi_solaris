
#define N 50

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int getNumOfNLsAndBytes(int argc, int* numOfNLs, int *numOfBytes, FILE * wcp)
{
        unsigned char buf[BUFSIZ];
        int numOfCharsRead;
        if (1 > (numOfCharsRead = fread(buf,sizeof(unsigned char),BUFSIZ - 1,wcp)))
        {
                perror("Wasn't able to read wc statistics");
                return 1;
        }
        buf[numOfCharsRead] = 0;
        int posOfChar = 0;
        int i, numOfStatsGot;
        int * currentStat;
        for (i = 1; i < argc; ++i)
        {
                numOfStatsGot = 0;
                currentStat = &numOfNLs[i];
                while (2 != numOfStatsGot)
                {
                        if ((buf[posOfChar] <= 57) && (buf[posOfChar] >= 48))
                        {
                                while ((buf[posOfChar] <= 57) && (buf[posOfChar] >= 48))
                                {
                                        *currentStat = 10 * (*currentStat) + buf[posOfChar] - '0';
                                        ++posOfChar;
                                }
                                --posOfChar;
                                ++numOfStatsGot;
                                currentStat = &numOfBytes[i];
                        }
                        ++posOfChar;
                        if (posOfChar == numOfCharsRead - 1)
                        {
                                fprintf(stderr,"Wasn't able to get number of new lines and bytes from files with wc");
                                return 1;
                        }
                }
		while (buf[posOfChar] != '\n')
			++posOfChar;
        }
        return 0;
}

int wc(int argc,char* argv[],int** numOfNLs,int** numOfBytes)
{
	unsigned char procName[N] = "wc -l -c";
	procName[N - 1] = 0;
        int i;
        for (i = 1; i < argc; ++i)
        {
                strcat(procName," ");
                strcat(procName,argv[i]);
        }
        FILE* wcp = popen(procName,"r");
        if (!wcp)
        {
                perror("Wasn't able to open process");
                return 1;
        }
	*numOfNLs = (int *)calloc(argc,sizeof(int));
	*numOfBytes = (int *)calloc(argc,sizeof(int));
	if ((!(*numOfNLs)) || (!(*numOfBytes)))
	{
		perror("Wasn't able to allocate memory for numOfBytes or numOfNLs arrays");
		return 1;
	}
	int result = getNumOfNLsAndBytes(argc,*numOfNLs,*numOfBytes,wcp);
	pclose(wcp);
	return result;
}

int findEmptyLines(int argc, char* argv[],int* numOfNLs, int* numOfBytes)
{
	int i, j, numberOfEmptyLines, numberOfCharsRead;
        unsigned char buf[BUFSIZ];
	buf[BUFSIZ - 1] = 0;
	FILE * f;
	for (i = 1; i < argc; ++i)
        {
		numberOfEmptyLines = numOfNLs[i];
                if (!(f = fopen(argv[i],"r")))
                {
                        fprintf(stderr,"Wasn't able to open [%s]",argv[i]);
                        return 1;
                }
                for (j = 0; j <= numOfNLs[i]; ++j)
                {
			if (numOfBytes[i] == numOfNLs[i] - j)
				break;
			fgets(buf,BUFSIZ - 1, f); 
                        numberOfCharsRead = strlen(buf);
			if (-1 == numberOfCharsRead)
			{
				fprintf(stderr,"Wasn't able to read line from [%s]",argv[i]);
				fclose(f);
				return 1;
			}
			if (1 < numberOfCharsRead)
				--numberOfEmptyLines;
			numOfBytes[i] -= numberOfCharsRead;
                }
		printf("in [%s]:\t%d empty line",argv[i],numberOfEmptyLines + 1);
		if (1 < numberOfEmptyLines)
			printf("s");
		printf("\n");
                fclose(f);
        }
	return 0;
}

int main(int argc, char* argv[])
{
	int* numOfNLs,* numOfBytes;
	if (wc(argc,argv,&numOfNLs,&numOfBytes))
		return 1;
	int result = findEmptyLines(argc,argv,numOfNLs,numOfBytes);
	return result;
}

