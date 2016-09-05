#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#define DATA "message through the tunnel!"
#define FILE_PATH "/home/george/Documents/C/pipe64K.txt"


main(argc, argv)
    int argc;
    char *argv[];
{
    int a;
    int sock, msgsock, rval, fd;
    struct sockaddr_un server;
    int bytes = 65536;
    char buf[bytes];
    FILE *fp2;    //write file

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

    struct timeval start, end;


    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("opening stream socket");
        //exit(1);
    }
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, argv[1]);

    if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
        close(sock);
        perror("connecting stream socket");
        //exit(1);
    }
    for( a = 1; a < 60; a += 1 ) {
        fp2=fopen(FILE_PATH,"a");


       gettimeofday(&start, NULL);
        write(sock, string, sizeof(string));
        //listen(sock, 5);
        //msgsock = accept(sock, 0, 0);
        //read(msgsock, buf, 1);
        gettimeofday(&end, NULL); //

        sleep(1);   
        long double time = ((long double)(end.tv_sec - start.tv_sec) + 
              ((long double)(end.tv_usec - start.tv_usec)/1000000.0));
        printf("%Lf\n", time);
        fprintf(fp2, "%Lf\n",time);
	      fflush(fp2);
        fclose(fp2);
    }
    close(sock);
}
