#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

#define DATA "message through the tunnel!"


main(argc, argv)
    int argc;
    char *argv[];
{
    int a=0;
    int sock, msgsock;
    struct sockaddr_un server;
    char buf[1024];

    if (argc < 2) {
        printf("usage:%s <pathname>", argv[0]);
        //exit(1);
    }

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
    for( a = 1; a < 10; a += 1 ) {
        msgsock = accept(sock, 0, 0);
        read(msgsock, buf, 1);
        write(sock, DATA, sizeof(DATA));
        sleep(1);
    }
    close(sock);
}
