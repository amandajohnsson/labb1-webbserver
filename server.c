#include <stdlib.h> /* server code*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/sendfile.h>

#define SERVER_PORT 12345
#define BUF_SIZE 4096
#define QUEUE_SIZE 10

char web_page[] =
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n\r\n"
    "<!DOCTYPE html>\r\n"
    "<html><head><title>ShellUaveX</title>\r\n"
    "<style>body {background-color: #FFA3FF }</style></head>\r\n"
    "<body><center><h1>Kossan säger muuu!</h1><br>\r\n"
    "<img src=\"ko.jpg\"></center></body></html>\r\n";

void fatal(char *string)
{
    printf("%s", string);
    exit(1);
}

int main(int argc, char *argv[])
{
    int server, b, l, fd, sa, bytes, on = 1;
    char buf[BUF_SIZE];
    struct sockaddr_in channel;

    /*build address atructure to bind to socket */

    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    channel.sin_addr.s_addr = htonl(INADDR_ANY);
    channel.sin_port = htons(SERVER_PORT);

    /* Passive open. wait for connection. */

    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /*create socket*/
    if (server < 0)
        fatal("socket failed");
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

    b = bind(server, (struct sockaddr *)&channel, sizeof(channel));
    if (b < 0)
    {
        fatal("bind failed");
        close(server);
    }
    l = listen(server, QUEUE_SIZE); /*specify queue size */
    if (l < 0)
    {
        fatal("listen failed");
        close(server);
    }
    /*Socket is now set up and bound.  Wait for connection and process it. */
    while (1)
    {
        printf("Waiting on connection\n");
        sa = accept(server, NULL, NULL); /*block for connection request */
        if (sa < 0)
            fatal("accept failed");
        printf("Connected\n");

        memset(buf, 0, BUF_SIZE);
        read(sa, buf, BUF_SIZE); /*READ FILE NAME FROM SOCKET*/

        /* get and return the file */

        /*if (!strncmp(buf, "GET /favicon.ico", 16))
        {
            fd = open("favicon.ico", O_RDONLY); /* open the file to be sent back */
        /*if (fd < 0)
                fatal("open failed");
            sendfile(sa, fd, NULL, BUF_SIZE);
        }*/
        printf("Doing compare. Buf is:\n");
        printf("%s", buf);
        printf("END OF BUF\n");

        if (!strncmp(buf, "GET /ko.jpg", 11))
        {
            printf("\n\n I if!\n\n");
            fd = open("ko.jpg", O_RDONLY); /* open the file to be sent back */

            if (fd < 0)
                fatal("open failed");

            char imageHeaders[] =
                "HTTP/1.0 200 Ok\r\n"
                "Content-Type: image/jpg\r\n"
                "Content-Length: 256564\r\n\r\n";

            write(sa, imageHeaders, sizeof(imageHeaders) - 1);
            while (1)
            {
                bytes = read(fd, buf, BUF_SIZE); //read from file
                if (bytes <= 0)
                    break;             //check for end of file
                write(sa, buf, bytes); //write bytes to socket
            }
            //sendfile(sa, fd, NULL, 300000);
            close(fd);
        }
        else
        {
            printf("I else\n");
            write(sa, web_page, sizeof(web_page) - 1);
        }

        //printf("%s", buf);
        //shutdown(sa, SHUT_WR);

        close(sa); /*close connection*/
    }
    return 0;
}
