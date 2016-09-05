#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/time.h>

#define FILEPATH "/home/george/Documents/C/mmapped.bin"
#define FILE_PATH "/home/george/Documents/C/pipe.txt"
#define NUMINTS  (4)
#define FILESIZE (NUMINTS * sizeof(int))



int main(int argc, char *argv[])
{
    int i;
    int fd;
    int result;
    int *map;  /* mmapped array of int's */

    struct timeval start, end;

    FILE *fp2;		//write to file

    int iter = 0;
    for(iter; iter<100; iter++){
        fp2=fopen(FILE_PATH,"a");
        /* Open a file for writing.
         *  - Creating the file if it doesn't exist.
         *  - Truncating it to 0 size if it already exists. (not really needed)
         * Note: "O_WRONLY" mode is not sufficient when mmaping.
         */
        fd = open(FILEPATH, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
        if (fd == -1) {
      perror("Error opening file for writing");
      exit(EXIT_FAILURE);
        }

        /* Stretch the file size to the size of the (mmapped) array of ints
         */
        result = lseek(fd, FILESIZE-1, SEEK_SET);
        if (result == -1) {
      close(fd);
      perror("Error calling lseek() to 'stretch' the file");
      exit(EXIT_FAILURE);
        }
        
        /* Something needs to be written at the end of the file to
         * have the file actually have the new size.
         * Just writing an empty string at the current file position 
         *    will be written at the last byte of the file.
         */
        result = write(fd, "", 1);
        if (result != 1) {
      close(fd);
      perror("Error writing last byte of the file");
      exit(EXIT_FAILURE);
        }


        /* Now the file is ready to be mmapped.
         */
        map = mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED) {
	    close(fd);
	    perror("Error mmapping the file");
	    exit(EXIT_FAILURE);
        }


       gettimeofday(&start, NULL);
        
        /* Now write int's to the file as if it were memory (an array of ints).      */
        for (i = 1; i <=NUMINTS; ++i) {
	    map[i] = 1; 
        }

        map = mmap(0, FILESIZE, PROT_READ, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED) {
	          close(fd);
	          perror("Error mmapping the file");
	          exit(EXIT_FAILURE);
        }
        
        /* Read the file int-by-int from the mmap
         */
        for (i = 1; i <=NUMINTS-1; i = i+NUMINTS/2) {
	          printf("%d ", map[i]);
        }


        gettimeofday(&end, NULL);
        long double time = ((long double)(end.tv_sec - start.tv_sec) + 
              ((long double)(end.tv_usec - start.tv_usec)/1000000.0));
        printf("%Lf\n", time);

        map = mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        map = mmap(0, FILESIZE, PROT_READ, MAP_SHARED, fd, 0);
        close(fd);	
        fprintf(fp2, "%Lf\n",time);
	      fflush(fp2);

        
        /* Don't forget to free the mmapped memory
         */
        if (munmap(map, FILESIZE) == -1) {
	      perror("Error un-mmapping the file");

        }

        fclose(fp2);
    }
    return 0;
}
