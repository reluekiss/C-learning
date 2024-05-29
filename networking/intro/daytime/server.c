#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
// Book makes use of wrapper functions which does error checking ie socket 
// would be Socket and would do standard handling of error messages,
int main(int argc, char **argv)
{
    int MAXLINE = 80;
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buff [MAXLINE] ;
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(13); /* daytime server */
    
    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    
    listen(listenfd, 80);
    
    for ( ; ; ) {
        connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));
        
        close(connfd);
    }
}
