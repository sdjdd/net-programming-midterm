#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void error(char *s, int status);
size_t recvstr(int fd, char *buf, size_t buf_len);
