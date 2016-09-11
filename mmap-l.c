#include <stdio.h>
#include "mythreads.h"
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define FILEPATH "/home/george/Documents/C/mmapped.bin"
#define NUMINTS  (10)
#define FILESIZE (NUMINTS * sizeof(int))

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
//long long count = 0;
int availA, availB = 0;
int *map;  /* mmapped array of int's */

void *
increment_count(void *arg)
{
    int iter, i;
    char *letter = arg;
    availA = 1;
    if (strcmp(letter, "A") == 0)
    {
        for(iter = 0; iter < 100; iter++)
        {
            pthread_mutex_lock(&lock);
            while (availA == 0)
                pthread_cond_wait(&cond, &lock);
            printf("%s running\n", letter);


            for (i = 0; i < NUMINTS; i++) {
	              map[i] = iter ; 
            }


            availA = 0;
            availB = 1;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&lock);
        }
    }
    else
    {
        for(iter = 0; iter < 100; iter++)
        {
            pthread_mutex_lock(&lock);
            while (availB == 0)
                pthread_cond_wait(&cond, &lock);
            printf("%s running\n", letter);


            printf("map[2]: %d\n", map[2]);


            availA = 1;
            availB = 0;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&lock);
        }
    }

    printf("%s: done\n", letter);
    return NULL;
}

/*long long
get_count()
{
    long long c;    
    pthread_mutex_lock(&lock);
	    c = count;
    pthread_mutex_unlock(&lock);
	    return (c);
}*/

/*void *
mythread(void *arg) {
    printf("%s\n", (char *) arg);
    return NULL;
}*/

int
main(int argc, char *argv[])
{              
    int fd;
    int result;

    //struct timeval start, end;

    //FILE *fp2;		//write to file

    //int file_save = 0;
    //for(file_save; file_save < 100 ; file_save++){
        //fp2=fopen(FILE_PATH,"a");
        fd = open(FILEPATH, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

    result = lseek(fd, FILESIZE-1, SEEK_SET);
    result = write(fd, "", 1);
    map = mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    pthread_t p1, p2;
    printf("main: begin\n");
    Pthread_create(&p1, NULL, increment_count, "A"); 
    Pthread_create(&p2, NULL, increment_count, "B");
    // join waits for the threads to finish
    Pthread_join(p1, NULL); 
    Pthread_join(p2, NULL); 
    printf("main: end");

    if (munmap(map, FILESIZE) == -1) {
	      perror("Error un-mmapping the file");
    }

    /* Un-mmaping doesn't close the file, so we still need to do that.
     */
    close(fd);

    return 0;
}

