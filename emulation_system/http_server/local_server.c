#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

#define RECVMAX 256

int main(int argc, char *argv[]) {
    int s, cs, cddr_len, length;
    struct sockaddr_in addr, caddr;
    char buf[RECVMAX];

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    // inet_aton("192.168.0.100", &addr.sin_addr);
    inet_aton("127.0.0.1", &addr.sin_addr);

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if(listen(s, 5)) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    cddr_len = sizeof(caddr);
    for (;;) {
        if ((cs = accept(s, (struct sockaddr *)&caddr, &cddr_len)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "\nconnection established\n");
        for (;;) {
			bzero(&buf, RECVMAX);
            if ((length = recv(cs, buf, RECVMAX, 0)) > 0) {
                fprintf(stdout, "%s", buf);
            } else {
                fprintf(stdout, "\nconnection closed\n");
                break;
            }
        }
        close(cs);
    }
    close(s);

    return 0;
}