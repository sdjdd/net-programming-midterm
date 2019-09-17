#include "common.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usage: %s [host] [port]\n", argv[0]);
        return 1;
    }

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_family = AF_INET;

    socklen_t server_len = sizeof(server_addr);
    if (connect(socket_fd, (struct sockaddr *) &server_addr, server_len) < 0) {
        error("connect", 1);
    }

    fd_set allreads;
    FD_ZERO(&allreads);
    FD_SET(0, &allreads);
    FD_SET(socket_fd, &allreads);

    printf("type \"quit\" to quit\n");

    char send_buf[BUFFER_SIZE], recv_buf[BUFFER_SIZE];
    int n, want_quit = 0;
    for (;;) {
        fd_set readmask = allreads;
        if (select(socket_fd + 1, &readmask, NULL, NULL, NULL) < 0) {
            error("select", 1);
        }
        if (FD_ISSET(socket_fd, &readmask)) {
            n = recvstr(socket_fd, recv_buf, BUFFER_SIZE);
            if (n < 0) {
                error("read", 1);
            } else if (n == 0) {
                if (want_quit) {
                    break;
                }
                printf("server terminated\n");
                return 1;
            }
            printf("%s\n", recv_buf);
        } else if (FD_ISSET(0, &readmask)) {
            if (fgets(send_buf, BUFFER_SIZE, stdin) == NULL) {
                continue;
            }
            if (strcmp(send_buf, "quit\n") == 0) {
                want_quit = 1;
                FD_CLR(0, &allreads);
                if (shutdown(socket_fd, SHUT_WR) < 0) {
                    error("shutdown", 1);
                }
            } else {
                n = strlen(send_buf);
                if (send_buf[n - 1] == '\n') {
                    send_buf[n - 1] = 0;
                }

                if (send(socket_fd, send_buf, n, 0) < 0) {
                    error("send", 1);
                }
            }
        }
    }

    return 0;
}
