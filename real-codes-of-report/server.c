#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

#define DATA "message through the tunnel!"
#define NAME "socket"


main()
{
    int sock, msgsock, rval;
    struct sockaddr_un server;
    char buf[65536];


    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("opening stream socket");
        //exit(1);
    }
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, NAME);
    if (bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un))) {
        perror("binding stream socket");
        //exit(1);
    }
    printf("Socket has name %s\n", server.sun_path);
    listen(sock, 5);
    while (1) {
        msgsock = accept(sock, 0, 0);
        if (msgsock == -1){
            perror("accept");
		break;}
        else do {
            bzero(buf, sizeof(buf));
            if ((rval = read(msgsock, buf, sizeof(buf))) < 0)
                perror("reading stream message");
            else if (rval == 0){
                printf("Ending connection\n");
        close(msgsock);
    close(sock);
    unlink(NAME);}
            else 
                ; //write(sock, DATA, sizeof(DATA));
        } while (rval > 0);
    }
}
