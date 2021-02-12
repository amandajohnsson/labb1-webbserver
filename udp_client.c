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
    int sd, server_length, b, request, receve;
    time_t start_time, server_time, current_time;
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

    b = bind(sd, (struct sockaddr *)&client_addr, sizeof(client_addr));
    if (b < 0)
    {
        fatal("bind failed\n");
        close(sd);
    }

    server_length = (int)sizeof(struct sockaddr);

    start_time = time(NULL);
    request = sendto(sd, &buf, BUF_SIZE, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (request < 0)
        fatal("Error sending data\n");
    receve = recvfrom(sd, &server_time, sizeof(server_time), 0, (struct sockaddr *)&serv_addr, &server_length);
    if (receve < 0)
        fatal("Error receiving data\n");

    current_time = time(NULL) - start_time;
    server_time += current_time / 2;
    printf("Server's Time: %s\n", ctime(&server_time));
    close(sd);
    return 0;
}