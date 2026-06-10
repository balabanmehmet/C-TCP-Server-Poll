/*
 * client.c — Basit TCP sohbet istemcisi (poll surumu)
 * Akis: socket() -> connect() -> poll() dongusu
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>

#define PORT      8080
#define BUF_SIZE  1024

int main(int argc, char *argv[])
{
    const char *server_ip = (argc > 1) ? argv[1] : "127.0.0.1";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); exit(1); }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(PORT);

    if (inet_pton(AF_INET, server_ip, &addr.sin_addr) <= 0) {
        fprintf(stderr, "Gecersiz IP adresi: %s\n", server_ip);
        exit(1);
    }

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect"); exit(1);
    }
    printf("Sunucuya baglanildi (%s:%d). Yazmaya baslayabilirsin.\n",
           server_ip, PORT);

    struct pollfd fds[2];
    char buf[BUF_SIZE];

    while (1) {
        fds[0].fd = 0;            /* klavye */
        fds[0].events = POLLIN;
        fds[1].fd = sock;         /* soket */
        fds[1].events = POLLIN;

        int ret = poll(fds, 2, -1);
        if (ret < 0) { perror("poll"); break; }

        /* a) Klavyeden -> sunucuya gonder */
        if (fds[0].revents & POLLIN) {
            if (fgets(buf, sizeof(buf), stdin) == NULL)
                break;                          /* EOF -> cik */
            send(sock, buf, strlen(buf), 0);
        }

        /* b) Sunucudan -> ekrana bas */
        if (fds[1].revents & POLLIN) {
            int n = recv(sock, buf, sizeof(buf) - 1, 0);
            if (n <= 0) {
                printf("Sunucu baglantiyi kapatti.\n");
                break;
            }
            buf[n] = '\0';
            printf("Sunucu: %s", buf);
            fflush(stdout);
        }
    }

    close(sock);
    return 0;
}