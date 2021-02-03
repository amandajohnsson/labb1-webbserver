#include <stdlib.h>   /* server code*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>

#define SERVER_PORT 12345
#define BUF_SIZE 4096
#define QUEUE_SIZE 10

int main(int argc, char* argv[]){
  int s,b,l, fd, sa, bytes, on =1;
  char buf[BUF_SIZE];
  struct sockaddr_in channel;

  /*build address atructure to bind to socket */

  memset(&channel, 0, sizeof(channel));
  channel.sin_family = AF_INET;
  channel.sin_addr.s_addr = htonl(INADDR_ANY);
  channel.sin_port = htons(SERVER_PORT);

/* Passive open. wait for connection. */

s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /*create socket*/
if(s<0) fatal("socket failed");
setsockopt(s,SOL_SOCKET, SO_REUSEADDR(char *) &on, sizeof(on));

b=bind(s, (struct sockaddr *) &channel, sizeof(channel));
if(b<0) fatal("bind failed");

l = listen(s, QUEUE_SIZE); /*specify queue size */
if(l < 0) fatal("listen failed");

/*Socket is now set up and bound.  Wait for connection and process it. */
while(1){
  sa=accept(s, 0,0); /*block for connection request */
  if(sa<0)fatal("accept failed");

  read(sa,buf,BUF_SIZE); /*READ FILE NAME FROM SOCKET*/

  /* get and return the file */

  fd = open(buf, O_RDONLY); /* open the file to be sent back */
  if(fd < 0) fatal("open failed");

  while(1){
    bytes=read(fd,buf, BUF_SIZE); /*read from file*/
    if(bytes <=0) break;          /*check for end of file */
    write(sa, buf, bytes);        /*write bytes to socket */
  }
  close(fd);                      /*close file*/
  close(sa);                       /*close connection*/
  }

}
//hello
