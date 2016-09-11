#include <stdio.h>
#include "mythreads.h"
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
long long count = 0;
int availA, availB = 0;

void *
increment_count(void *arg)
{
    int iter;
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
            count = count + 1;
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
            count = count + 1;
            availA = 1;
            availB = 0;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&lock);
        }
    }

    printf("%s: done\n", letter);
    return NULL;
}

long long
get_count()
{
    long long c;    
    pthread_mutex_lock(&lock);
	    c = count;
    pthread_mutex_unlock(&lock);
	    return (c);
}

/*void *
mythread(void *arg) {
    printf("%s\n", (char *) arg);
    return NULL;
}*/

int
main(int argc, char *argv[])
{                    
    if (argc != 1) {
	  fprintf(stderr, "usage: main\n");
	  exit(1);
      }

    pthread_t p1, p2;
    printf("main: begin\n");
    Pthread_create(&p1, NULL, increment_count, "A"); 
    Pthread_create(&p2, NULL, increment_count, "B");
    // join waits for the threads to finish
    Pthread_join(p1, NULL); 
    Pthread_join(p2, NULL); 
    printf("main: end \nget_count: %lld\n", get_count());
    return 0;
}

