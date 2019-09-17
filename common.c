#include "common.h"

void error(char *s, int status) {
    perror(s);
    exit(status);
}

size_t read_msg(int fd, char *buf, size_t buf_len) {
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
        if (*buf == 3) {  // <ETX>
            // *buf = 0;  // should process by caller
            return count;
        }
        ++p;
        ++buf;
        --nleft;
        --buf_len;
    }

    return -1;
}

size_t send_msg(int fd, char *buf, size_t buf_len) {
    int len = strlen(buf);
    if (buf[len - 1] == '\n') {
        --len;
    } else if (len < buf_len) {

    }
    buf[len - 1] = 3;
    return send(fd, buf, len)
}
