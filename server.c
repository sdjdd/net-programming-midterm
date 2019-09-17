#include "common.h"
#include <signal.h>
#include <dirent.h>
#include <unistd.h>

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

        char buffer[BUFFER_SIZE];
        for (;;) {
            int n;
            n = recvstr(connfd, buffer, BUFFER_SIZE);
            if (n < 0) {
                error("read", 1);
            } else if (n == 0) {
                printf("client logout\n");
                send(connfd, "bye~", 5, 0);
                shutdown(connfd, SHUT_WR);
                break;
            }

            printf("received: %s\n", buffer);

            if (strcmp(buffer, "?") == 0) {
                strcpy(buffer, "commands:\n pwd cd ls");
            } else if (strcmp(buffer, "pwd") == 0) {
                if (getcwd(buffer, BUFFER_SIZE) == NULL) {
                    error("getcwd", 1);
                }
            } else if (strncmp(buffer, "cd", 2) == 0) {
                if (strlen(buffer) <= 3) {
                    strcpy(buffer, "usage: cd [path]");
                } else if (chdir(buffer + 3) != 0) {
                    strcpy(buffer, strerror(errno));
                } else {
                    continue;
                }
            } else if (strcmp(buffer, "ls") == 0) {
                DIR *d;
                struct dirent *dir;
                d = opendir(".");
                if (d == NULL) {
                    error("opendir", 1);
                }
                char *p = buffer;
                while ((dir = readdir(d)) != NULL) {
                    strcpy(p, dir->d_name);
                    p += dir->d_namlen;
                    if (p - buffer >= BUFFER_SIZE - 1) {
                        printf("ls too long\n");
                        return 1;
                    }
                    *p++ = '\n';
                }
                *(p - 1) = 0;
                closedir(d);
            } else {
                strcpy(buffer, "unknown command. type \"?\" to show all commands");
            }

            if (send(connfd, buffer, strlen(buffer) + 1, 0) < 0) {
                error("send", 1);
            }
        }
    }

    return 0;
}
