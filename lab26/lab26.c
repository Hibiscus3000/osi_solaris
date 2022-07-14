#define N 30

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	unsigned char rf[N] = "./lab26_r.out";
	unsigned char wf[N] = "./lab26_w.out";
	FILE * r = popen(rf,"r");
	if (!r)
	{
		perror("Wasn't able to open reading process");
		return 1;
	}
	FILE * w = popen(wf,"w");
	if (!w)
	{
		perror("Wasn't able to open writing process");
		return 1;
	}
	unsigned char* buf = (unsigned char*)malloc(BUFSIZ * sizeof(unsigned char));
	int numberOfCharsRead;
	if (1 > (numberOfCharsRead = fread(buf,sizeof(unsigned char),BUFSIZ - 1,r)))
	{
		perror("Wasn't able to read line");
		free(buf);
		pclose(r);
		pclose(w);
		return 1;
	}
	buf[numberOfCharsRead] = 0;
	if (1 > fwrite(buf,sizeof(unsigned char),numberOfCharsRead,w))
        {
                perror("Wasn't able to write line");
                free(buf);
		pclose(r);
                pclose(w);
                return 1;
        }
	free(buf);
	pclose(r);
	pclose(w);
	return 0;
}
