#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include </usr/include/linux/fcntl.h>
#include <unistd.h>
#define FILE_PATH "/home/george/Documents/C/pipe64K.txt"

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
	int bytes = 4;

    char buf[bytes];
    int n2;

    FILE *fp;           //read file
    char str[bytes];
    char string[bytes];
    fp = fopen("512KB.txt" , "r");
    if(fp == NULL) 
    {
      perror("Error opening file");
      return(-1);
    }
    if( fgets (str, bytes, fp)!=NULL ) 
    {
        memcpy(string, str, bytes);
        string[bytes] = '\0'; 
        //printf("%s\n", string);
    }
    fclose(fp);

    char *data1 = string;
    char *data2 = "k"; 	//to be changed

    FILE *fp2;		//write to file
    if( (fp2=fopen(FILE_PATH,"wt+")) == NULL ){
        perror(FILE_PATH);
        exit(1);
    }
    int i = 0;
    for(i; i<100; i++){

	pipe(fd1);
	pipe(fd2);

	x = rdtsc();        //start to count time

	switch (pid = fork()) {

	case 0: // child 
		runpipe(fd1, fd2, data1, data2, buf);
		exit(0);

	default: // parent
		while ((pid = wait(&status)) != -1)
            ;

	    close(fd2[1]);	
	    if ((n2 = read(fd2[0], buf, strlen(data2))) >= 0) {
		    buf[n2] = 0;	// terminate string 
		    
            printf("read %d bytes from the pipe2 \n", n2);
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
        long double time = (long double)(y-x) / 2600000000;
	// round-trip
	printf("time spend (half of bi-direction): %Lf\n",time);
	fprintf(fp2, "%Lf\n",time);
	fflush(fp2);
    }
	fclose(fp2);
	exit(0);
}

int n1;

void
runpipe(int fd1[], int fd2[], char *data1, char *data2, char buf[])
{
	int pid;

	switch (pid = fork()) {

	case 0: /* child */
	    close(fd2[0]);	
	    write(fd2[1], data2, strlen(data2));

		close(fd1[1]);	
		if ((n1 = read(fd1[0], buf, strlen(data1))) >= 0) {
			buf[n1] = 0;	/* terminate string */
		
            printf("read %d bytes from the pipe1\n", n1);
		}	
		else
			perror("read");
		exit(0);

	default: /* parent */
		close(fd1[0]);	
		write(fd1[1], data1, strlen(data1));

	case -1:
		perror("fork");
		exit(1);
	}
}

