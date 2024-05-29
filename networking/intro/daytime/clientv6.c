#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int sockfd, n;
    int MAXLINE = 80;
    char recvline[MAXLINE + 1];
    struct sockaddr_in6 servaddr;

    if (argc != 2) {
    	perror("usage: a.out <IPaddress>");
    }

    if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
    }

    memset(&servaddr, 0, sizeof (servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_port = htons(13); /* daytime server */
    
    if (inet_pton(AF_INET6, argv[1], &servaddr.sin6_addr) <= 0) {
        printf("inet_pton error for %s", argv[1]);
    }
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
    }

    while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] =0; /* null terminate */
        if (fputs(recvline, stdout) == EOF) {
            perror("fputs error");
        }
    }
     
    if (n < 0) {
         perror("read error");
    }
    return 0;
}
