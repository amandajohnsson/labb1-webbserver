#include <stdlib.h> /* server code*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
//#include <sys/sendfile.h>

#define SERVER_PORT 12345
#define BUF_SIZE 4096
#define QUEUE_SIZE 10

char *parsebuf(char *string)
{

    return string;
}

void fatal(char *string)
{
    printf("%s", string);
    exit(1);
}

void send_file(int sa, int fd)
{

    int bytes;
    char buf[BUF_SIZE];

    while (1)
    {
        bytes = read(fd, buf, BUF_SIZE); //read from file
        if (bytes <= 0)
            break;             //check for end of file
        write(sa, buf, bytes); //write bytes to socket
    }
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
        printf("Connected\n\n");

        memset(buf, 0, BUF_SIZE);
        read(sa, buf, BUF_SIZE); /*READ FILE NAME FROM SOCKET*/

        /* get and return the file */

        printf("Doing compare. Buf is:\n");
        printf("%s\n", buf);
        printf("END OF BUF\n");

        if (!strncmp(buf, "GET / ", 6))
        {

            char web_page_Header[] =
                "HTTP/1.0 200 OK\r\n"
                "Content-Type: text/html; charset=UTF-8\r\n\r\n";
            write(sa, web_page_Header, sizeof(web_page_Header) - 1);

            fd = open("index.html", O_RDONLY); /* open the file to be sent back */
            if (fd < 0)
                fatal("open failed");

            send_file(sa, fd);

            close(fd);
        }

        else if (!strncmp(buf, "GET /%22ko.jpg/%22", 18))
        {
            printf("\n\n I if!\n\n");
            fd = open("ko.jpg", O_RDONLY); /* open the file to be sent back */

            if (fd < 0)
                fatal("open failed");

            char imageHeaders[] =
                "HTTP/1.0 200 Ok\r\n"
                "Content-Type: image/jpg\r\n"
                "Content-Length: 178541\r\n\r\n";

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
        else if (!strncmp(buf, "GET /%22kogif.gif/%22", 21))
        {

            fd = open("kogif.gif", O_RDONLY);
            if (fd < 0)
                fatal("open failed");

            char imageHeaders[] =
                "HTTP/1.0 200 Ok\r\n"
                "Content-Type: image/gif\r\n"
                "Content-Length: 32044\r\n\r\n";

            write(sa, imageHeaders, sizeof(imageHeaders) - 1);

            send_file(sa, fd);

            printf("\n\n");

            close(fd);
        }
        else if (!strncmp(buf, "GET /%22felko.jpg/%22 ", 22))
        {

            fd = open("felko.jpg", O_RDONLY); /* open the file to be sent back */

            printf("FD = %d\n", fd);
            if (fd < 0)
                fatal("open failed");

            char imageHeaders[] =
                "HTTP/1.0 200 Ok \r\n"
                "Content-Type: image/jpg\r\n"
                "Content-Length: 316226\r\n\r\n";

            write(sa, imageHeaders, sizeof(imageHeaders) - 1);

            send_file(sa, fd);

            printf("\n\n");

            close(fd);
        }
        else
        {
            char web_page_Header[] =
                "HTTP/1.0 400 Not Found\r\n"
                "Content-Type: text/html; charset=UTF-8\r\n\r\n";

            write(sa, web_page_Header, sizeof(web_page_Header) - 1);

            fd = open("errorpage.html", O_RDONLY); /* open the file to be sent back */
            if (fd < 0)
                fatal("open failed");

            send_file(sa, fd);

            close(fd);
        }

        close(sa); /*close connection*/
    }
    return 0;
}
