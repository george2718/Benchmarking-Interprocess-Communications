#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

inline uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ (
      "xorl %%eax, %%eax\n"
      "cpuid\n"
      "rdtsc\n"
      : "=a" (lo), "=d" (hi)
      :
      : "%ebx", "%ecx");
    return (uint64_t)hi << 32 | lo;
}



void runpipe();

int
main(int argc, char **argv)
{

	unsigned long long x;
	unsigned long long y;
	int pid, status;
	int fd1[2];
	int fd2[2];

    //char *data = "hello... this is sample data";
    char buf[10250];
    int n2;

    FILE *fp;           //read file
    char str[6000];
    char string[6010];
    fp = fopen("512KB.txt" , "r");
    if(fp == NULL) 
    {
      perror("Error opening file");
      return(-1);
    }
    if( fgets (str, 6000, fp)!=NULL ) 
    {
        memcpy(string, str, 6000);
        string[6000] = '\0'; 
        //printf("%s\n", string);
    }
    fclose(fp);

    char *data = string;

	pipe(fd1);
	pipe(fd2);

	x = rdtsc();        //start to count time

	switch (pid = fork()) {

	case 0: // child 
		runpipe(fd1, fd2, data, buf);
		exit(0);

	default: // parent
		while ((pid = wait(&status)) != -1)
            fprintf(stderr, "process %d exits\n", pid);

	    close(fd2[1]);	
	    if ((n2 = read(fd2[0], buf, 1024)) >= 0) {
		    buf[n2] = 0;	// terminate string 
		    //printf("read %d bytes from the pipe: \"%s\"\n", n2, buf);
            printf("read %d bytes from the pipe: \"\"\n", n2);
	    }	
	    else
		    perror("read");

		break;

	case -1:
		perror("fork");
		exit(1);
	}

	y = rdtsc(); //end counting time   
	printf("total cycles for round trip: %lld\n",y-x);
    long double time = (long double)(y-x) / 5200000000;
	printf("time spend for half of bi-direction: %Lf\n",time);
	exit(0);
}

int n1;

void
runpipe(int fd1[], int fd2[], char *data, char buf[])
{
	int pid;

	switch (pid = fork()) {

	case 0: /* child */
	    close(fd2[0]);	
	    write(fd2[1], data, strlen(data));

		close(fd1[1]);	
		if ((n1 = read(fd1[0], buf, 1024)) >= 0) {
			buf[n1] = 0;	/* terminate string */
			//printf("read %d bytes from the pipe: \"%s\"\n", n1, buf);
            printf("read %d bytes from the pipe: \"\"\n", n1);
		}	
		else
			perror("read");
		exit(0);

	default: /* parent */
		close(fd1[0]);	
		write(fd1[1], data, strlen(data));

	case -1:
		perror("fork");
		exit(1);
	}
}

