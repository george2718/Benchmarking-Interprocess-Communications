#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{

  int i = 0;
  int iterations = 10000000;

  struct timeval start, end;

  gettimeofday(&start, NULL);

  for (i; i < iterations; i++)
    { ;
    }
  
  gettimeofday(&end, NULL);

  printf("%f\n", ((end.tv_sec - start.tv_sec) + 
              ((end.tv_usec - start.tv_usec)/1000000.0)));

  return 0;
}
