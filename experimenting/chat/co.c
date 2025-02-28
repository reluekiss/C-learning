#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "coroutine.h"

bool quit = false;

void serve(int sockfd, SSL_CTX *ctx)
{
    int clientfd = accept(sockfd, NULL, NULL);
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, clientfd);
    if (SSL_accept(ssl) <= 0) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(clientfd);

        quit = true;
        return;
    }
    char buf[1024];
    int r;
    while ((r = SSL_read(ssl, buf, sizeof(buf))) > 0) {
        SSL_write(ssl, buf, r);
    }
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(clientfd);
}

int main(int argc, char** argv) {
    if (argc < 1) {
        printf("Usage: main <port>");
        exit(1);
    }
    int port = atoi(argv[0]);

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM);

    coroutine_init();
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("[error]: failed to bind port");
    };
    listen(sockfd, 1);

    while (!quit) {
        coroutine_sleep_read(sockfd);
        coroutine_go(serve());
    }
    
    coroutine_finish();
    close(sockfd);
    SSL_CTX_free(ctx);
    return 0;
}
