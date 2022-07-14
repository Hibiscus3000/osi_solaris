#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void lookup(char * sample,char * dirName, unsigned char* foundMatch);
int checkSpecChars(char * sample, char * entryName, int entryNameLength, int *currentSampleChar, int *currentNameChar, int sampleLength,
	unsigned char* foundMatch, char * dirName);
int processAsterisk(char * sample, char * entryName, int entryNameLength, int *currentSampleChar, int *currentNameChar, int sampleLength,
	unsigned char * foundMatch, char * dirName);
void processSlash(char * dirName, char * entryName, int currentNameChar, char * sample, int currentSampleChar,
	unsigned char * foundMatch);

int main(int argc, char* argv[])
{
	char sample[256];
	int rc;
	setbuf(stdout,NULL);
	printf("Please enter your sample here: ");
	if (0 > (rc = read(STDIN_FILENO,sample,BUFSIZ - 1)))
	{
		perror("Wasn't able to read sample");
		return 1;
	}
	sample[rc - 1] = 0;
	unsigned char foundMatch = 0;
	lookup(sample,"./",&foundMatch);
	if (!foundMatch)
		printf("No appropriate files found, given sample: %s\n",sample);
	return 0;
}

void lookup(char * sample, char * dirName, unsigned char* foundMatch)
{
	DIR *dirp;
        struct dirent *entry;

        if (!(dirp = opendir(dirName)))
                return;
	int entryNameLength;
	int specCharSearchRes;
	int sampleLength = strlen(sample);
	int currentSampleChar, currentNameChar;
	while (entry = readdir(dirp))
	{
		entryNameLength = strlen(entry->d_name);
		currentSampleChar = currentNameChar = 0;
		while (currentSampleChar < sampleLength)
		{
			if (!(specCharSearchRes = checkSpecChars(sample,entry->d_name,entryNameLength,&currentSampleChar,&currentNameChar,sampleLength,
				foundMatch,dirName)))
			{
				if ((entryNameLength == currentNameChar) || (sample[currentSampleChar] != (entry->d_name)[currentNameChar]))
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
			*foundMatch = 1;
			printf("%s%s\n",dirName,entry->d_name);
		}
	}
	closedir(dirp);
}

int checkSpecChars(char * sample, char * entryName, int entryNameLength, int *currentSampleChar, int *currentNameChar, int sampleLength,
	unsigned char* foundMatch, char * dirName)
{
	if ('/' == sample[*currentSampleChar])
	{
		processSlash(dirName,entryName,*currentNameChar,sample,*currentSampleChar,foundMatch);
		return 0;
	}
	if ('*' == sample[*currentSampleChar])
		return processAsterisk(sample,entryName,entryNameLength,currentSampleChar, currentNameChar, sampleLength,foundMatch,dirName);
	else if (*currentNameChar == entryNameLength)
		return 0;
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

void processSlash(char * dirName, char * entryName, int currentNameChar, char * sample, int currentSampleChar,
	unsigned char * foundMatch)
{
	char * nextDirName = (char *)malloc(sizeof(char) * 256);
	memcpy(nextDirName,dirName,strlen(dirName) + 1);
	strncat(nextDirName,entryName,currentNameChar + 1);
	strcat(nextDirName,"/");
	nextDirName[strlen(dirName) + currentNameChar + 2] = 0;
	lookup(&sample[currentSampleChar + 1],nextDirName,foundMatch);
	free(nextDirName);
}

int processAsterisk(char * sample, char * entryName, int entryNameLength, int *currentSampleChar, int *currentNameChar, int sampleLength,
	unsigned char * foundMatch, char * dirName)
{
	++(*currentSampleChar);
	while (*currentSampleChar < sampleLength)
	{
		if ('/' == sample[*currentSampleChar])
		{
			*currentNameChar = entryNameLength - 1;
			if ((strcmp(entryName,"..")) && (strcmp(entryName,".")))
				processSlash(dirName,entryName,*currentNameChar,sample,*currentSampleChar,foundMatch);
			return 2;
		}
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
							(sample[*currentSampleChar + addSampleChar] == '?') || (sample[*currentSampleChar + addSampleChar] == '/'))
						{
							*currentSampleChar += addSampleChar;
							*currentNameChar += addNameChar;
							if (sample[*currentSampleChar + addSampleChar] == '/')
							{
								if ((strcmp(entryName,"..")) && (strcmp(entryName,".")))
									processSlash(dirName,entryName,*currentNameChar,sample,*currentSampleChar,foundMatch);
								return 2;
							}
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
