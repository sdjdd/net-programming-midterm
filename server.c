#include "common.h"
#include <signal.h>
#include <dirent.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usage: %s [host] [port]\n", argv[0]);
        exit(1);
    }

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_family = AF_INET;

    if (bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        error("bind", 1);
    }

    if (listen(listenfd, 1024) < 0) {
        error("listen", 1);
    }

    signal(SIGPIPE, SIG_IGN);

    for (;;) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int connfd = accept(listenfd, (struct sockaddr *) &client_addr, &client_len);
        if (connfd < 0) {
            error("accept", 1);
        }
        printf("client login\n");

        char message[BUFFER_SIZE];
        for (;;) {
            int n;
            n = readline(connfd, message, BUFFER_SIZE);
            if (n < 0) {
                error("read", 1);
            } else if (n == 0) {
                printf("client logout\n");
                send(connfd, "bye~", 4, 0);
                shutdown(connfd, SHUT_RDWR);
                break;
            }

            message[n - 1] = '\0';
            printf("received: %s\n", message);

            n = send(connfd, message, n, 0);
            if (n < 0) {
                error("send", 1);
            }
        }
    }

    return 0;
}
