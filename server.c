#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>        /* close(), read() */
#include <arpa/inet.h>     /* sockaddr_in, htons, INADDR_ANY */
#include <sys/socket.h>  
#include <poll.h>


#define PORT  8080
#define BUF_SIZE  1024

int main(void){
    int server_fd = socket(AF_INET , SOCK_STREAM, 0);
    if(server_fd <0){
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr,0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if(bind(server_fd , (struct sockaddr *)&addr , sizeof(addr))< 0){
        perror("bind");
        exit(1);
    }

    if(listen(server_fd,1)<0){
        perror("listen");
        exit(1);
    }

    printf("Sunucu %d portunda dinliyor... istemci bekleniyor.\n",PORT);

    int client_fd = accept(server_fd , NULL ,NULL);
    if(client_fd < 0){
        perror("accept");
        exit(1);
    }
    printf("Istemci baglandi Yazmaaya baslayabilirsib.\n");
    
    struct pollfd fds[2];
    char buf[BUF_SIZE];
    while(1){
        

        fds[0].fd = 0;
        fds[0].events = POLLIN;


        fds[1].fd = client_fd;
        fds[1].events = POLLIN;


        int ret = poll(fds , 2 ,-1);

        if(ret<0){
            perror("poll");
            break;
        }


        if(fds[0].revents & POLLIN){
            
            fgets(buf , sizeof(buf),stdin);
            printf("[KLAVYE] Kullanıcı şunu yazdı: %s", buf);
        }
        
        if(fds[1].revents & POLLIN){
            int received = recv(fds[1].fd , buf , sizeof(buf)-1,0);
            if(received<=0){
                perror("bağlantı kapandı");
                break;
            }else{
                buf[received]= '\0';
                printf("[SOCKET] Sunucudan mesaj geldi: %s\n", buf);
            }

        }



        
    }
    close(client_fd);
    close(server_fd);
    return 0;
}