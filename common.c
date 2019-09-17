#include "common.h"

void error(char *s, int status) {
    perror(s);
    exit(status);
}

size_t recvstr(int fd, char *buf, size_t buf_len) {
    static char read_buf[4096];
    static char *p;
    int nleft = 0, count = 0;

    while (buf_len > 0) {
        if (nleft <= 0) {
            int nread = recv(fd, read_buf, sizeof(read_buf), 0);
            if (nread <= 0) {
                if (errno == EINTR) {
                    continue;
                }
                return nread;
            }
            p = read_buf;
            nleft = nread;
        }
        *buf = *p;
        ++count;
        if (*buf == 0) {
            return count;
        }
        ++p;
        ++buf;
        --nleft;
        --buf_len;
    }

    return -1;
}
