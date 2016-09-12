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
#include <time.h>
#include <sys/time.h>

#define FILE_PATH "/home/george/Documents/C/mmap-l.txt"
#define FILEPATH "/home/george/Documents/C/mmapped.bin"
#define NUMINTS  (262144)
#define FILESIZE (NUMINTS * sizeof(int))

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int availA, availB;
int *map;  /* mmapped array of int's */
struct timeval start, end;
FILE *fp2;		//write to file


void *
increment_count(void *arg)
{
    int i;
    char *letter = arg;
    availA = 1;
    availB = 0;
    int read;

    if (strcmp(letter, "A") == 0)
    {
        //for(iter = 0; iter < 1000; iter++)
        
            pthread_mutex_lock(&lock);
            while (availA == 0)
                pthread_cond_wait(&cond, &lock);
            //printf("%s running\n", letter);

            gettimeofday(&start, NULL); //start

            for (i = 0; i < NUMINTS; i++) {
	              map[i] = 3 ; 
            }


            //availA = 0;
            availB = 1;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&lock);
        
    }
    else
    {
        //for(iter = 0; iter < 100; iter++)
        
            pthread_mutex_lock(&lock);
            while (availB == 0)
                pthread_cond_wait(&cond, &lock);

            for (i = 0; i < NUMINTS; i++) {
	              read = map[i]; 
            }

            gettimeofday(&end, NULL);

            //availA = 1;
            //availB = 0;
            //pthread_cond_signal(&cond);
            pthread_mutex_unlock(&lock);
        
    }

    //printf("%s: done\n", letter);
    return NULL;
}

int
main(int argc, char *argv[])
{              
    int fd, file_save = 0;
    fp2 = fopen(FILE_PATH,"a");
    fd = open(FILEPATH, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    lseek(fd, FILESIZE-1, SEEK_SET);
    write(fd, "", 1);
    map = mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    for (file_save; file_save < 1000 ; file_save++)
    {

        pthread_t p1, p2;
        //printf("main: begin\n");


        Pthread_create(&p1, NULL, increment_count, "A"); 
        Pthread_create(&p2, NULL, increment_count, "B");
        Pthread_join(p1, NULL); //waits for the threads to finish
        Pthread_join(p2, NULL); 

        long double time = ((long double)(end.tv_sec - start.tv_sec) + 
              ((long double)(end.tv_usec - start.tv_usec)/1000000.0));
        //printf("%Lf\n", time);

        fprintf(fp2, "%Lf\n",time);
        fflush(fp2);

        //printf("main: end");
    }


    if (munmap(map, FILESIZE) == -1) {
	      perror("Error un-mmapping the file");
    }

    /* Un-mmaping doesn't close the file, so we still need to do that.
     */
    close(fd);
    fclose(fp2);
    return 0;
}

