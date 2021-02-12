#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <time.h>

#define SERVER_PORT 12345
#define BUF_SIZE 4096

void fatal(char *string)
{
    printf("%s", string);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sd, b, client_length, bytes, response;
    time_t curr_time;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_addr, client_addr;

    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sd < 0)
        fatal("socket failed\n");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERVER_PORT);

    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = htons(SERVER_PORT);

    b = bind(sd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (b < 0)
    {
        fatal("bind failed\n");
        close(sd);
    }

    client_length = (int)sizeof(struct sockaddr);

    while (1)
    {
        // receiving datagram from client
        bytes = recvfrom(sd, buf, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &client_length);
        if (bytes < 0)
        {
            fatal("Could not recive datagram\n");
        }
        curr_time = time(NULL);

        //Sending back response
        response = sendto(sd, &curr_time, sizeof(curr_time), 0, (struct sockaddr *)&client_addr, client_length);
        if (response < 0)
        {
            fatal("Sending back data failed\n");
        }
    }
    close(sd);
    return (0);
}