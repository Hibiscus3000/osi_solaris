#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void lookup(const char * sample);
int checkSpecChars(const char * sample, char * entryName, int entryNameLength, int *currentSampleChar, int *currentNameChar, int sampleLength);
int processAsterisk(const char * sample, char * entryName, int entryNameLength, int *currentSampleChar, int *currentNameChar, int sampleLength);

int main(int argc, char* argv[])
{
	char sample[BUFSIZ];
	int rc;
	setbuf(stdout,NULL);
	printf("Please enter your sample here: ");
	if (0 > (rc = read(STDIN_FILENO,sample,BUFSIZ - 1)))
	{
		perror("Wasn't able to read sample");
		return 1;
	}
	sample[rc - 1] = 0;
	lookup(sample);
	return 0;
}

void lookup(const char * sample)
{
	DIR *dirp;
        struct dirent *entry;

        if (!(dirp = opendir(".")))
        {
                perror("couldn't open '.'");
                return;
        }
	int entryNameLength;
	int specCharSearchRes;
	int sampleLength = strlen(sample);
	unsigned char foundMatch = 0;
	int currentSampleChar, currentNameChar;
	while (entry = readdir(dirp))
	{
		entryNameLength = strlen(entry->d_name);
		currentSampleChar = currentNameChar = 0;
		while (currentSampleChar < sampleLength)
		{
			if (-1 == (specCharSearchRes = checkSpecChars(sample,entry->d_name,entryNameLength,&currentSampleChar,&currentNameChar,
				sampleLength)))
			{
				printf("Sample cannot include '/', given sample: %s\n",sample);
				closedir(dirp);
				return;
			}
			else if (!specCharSearchRes)
			{
				if ((currentNameChar == entryNameLength) || (sample[currentSampleChar] != (entry->d_name)[currentNameChar]))
					break;
				else
				{
					++currentNameChar;
					++currentSampleChar;
				}
			}
			else if (2 == specCharSearchRes)
				break;
		}
		if ((2 != specCharSearchRes) && (currentSampleChar == sampleLength) && (currentNameChar == entryNameLength))
		{
			foundMatch = 1;
			printf("%s\n",entry->d_name);
		}
	}
	if (!foundMatch)
		printf("No appropriate files were found, given sample: %s\n",sample);
	closedir(dirp);
}

int checkSpecChars(const char * sample, char * entryName, int entryNameLength, int *currentSampleChar, int *currentNameChar, int sampleLength)
{
	if ('/' == sample[*currentSampleChar])
		return -1;
	if ('*' == sample[*currentSampleChar])
		return processAsterisk(sample,entryName,entryNameLength,currentSampleChar, currentNameChar, sampleLength);
	if ('?' == sample[*currentSampleChar])
	{
		if (*currentNameChar == entryNameLength)
			return 2;
		++(*currentSampleChar);
		++(*currentNameChar);
		return 1;
	}
	return 0;
}

int processAsterisk(const char * sample, char * entryName, int entryNameLength, int *currentSampleChar, int *currentNameChar, int sampleLength)
{
	++(*currentSampleChar);
	while (*currentSampleChar < sampleLength)
	{
		if ('/' == sample[*currentSampleChar])
			return -1;
		if ('?' == sample[*currentSampleChar])
		{
			++(*currentNameChar);
			if (*currentNameChar - 1 >= entryNameLength)
				return 2;
		}
		else if (('*' != sample[*currentSampleChar]) && (*currentSampleChar < sampleLength))
		{
			while (*currentNameChar < entryNameLength)
			{
				if (sample[*currentSampleChar] == entryName[*currentNameChar])
				{
					int addNameChar = 1;
					int addSampleChar = 1;
					while (*currentNameChar <= entryNameLength)
					{
						if ((*currentNameChar + addNameChar == entryNameLength) || (sample[*currentSampleChar + addSampleChar] == '*') ||
							(sample[*currentSampleChar + addSampleChar] == '?'))
						{
							*currentSampleChar += addSampleChar;
							*currentNameChar += addNameChar;
							return 1;
						}
						if ((addSampleChar + *currentSampleChar == sampleLength) || (sample[*currentSampleChar + addSampleChar]
							!= entryName[*currentNameChar + addNameChar]))
							break;
						++addNameChar;
						++addSampleChar;
					}
				}
				++(*currentNameChar);
			}
			return 2;
		}
		++(*currentSampleChar);
	}
	*currentNameChar = entryNameLength;
	return 1;
}
