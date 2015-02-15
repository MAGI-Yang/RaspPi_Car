//server.c
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/mman.h>

#define SERVER_PORT 5500
#define BUF_SIZE 512
#define LNUM 4

int main()
{
    int listen_fd, client_id, len = 1;
    struct sockaddr_in server_addr;
    pid_t pid;

    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Socket Error: %s.\n\a", strerror(errno));
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);    
	//允许地址重用，不需要等待TIME_WAIT，1为可重用，0为不可
	setsockopt( listen_fd, SOL_SOCKET, SO_REUSEADDR, &len, sizeof(len) );

    if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        printf("Bind Error: %s.\n\a", strerror(errno));
        exit(1);
    }
    listen(listen_fd, LNUM);

    struct sockaddr_in saddr;
    socklen_t saddrlen = sizeof(saddr);
    printf("Server is started !\n");

    fd_set master;      //主要的端口列表 Master port list
    fd_set read_fds;    //临时的端口列表 Temp port list
    int fdmax, i, newfd, j;
    char buf[BUF_SIZE + 1];
    char ip_buf1[30]="none\0";
    //char ip_buf2[30];

    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(listen_fd, &master);
    fdmax = listen_fd;  //现在只有一个监听的 Only one listerner now
    while(1){
        read_fds = master;
        if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
            perror("Server-select() error");
            exit(1);
        }

        for(i = 0; i <= fdmax; i++){    //遍历所有连接，读取数据 Traverse all connection and read data
            if(FD_ISSET(i, &read_fds)){ //有数据可读 If data is not null
                if(i == listen_fd){ //从监听端口来的 Get from listner port
                    if((newfd = accept(listen_fd, (struct sockaddr *)&saddr, &saddrlen)) == -1){
                        perror("Server-accept() error");
                    }else{
                        FD_SET(newfd, &master);     //加入到主端口列表 Add to Master port list
                        if(newfd > fdmax){
                            fdmax = newfd;      //保持fdmax为最大 Keep fdmax is max
                        }
                        sprintf(buf, "Your SocketID is: %d.last ip is:#%s#", newfd,ip_buf1);
                        send(newfd, buf, 51, 0);
                        printf("There is a new connection in ,from %s ,SocketID is %d.\n", inet_ntoa(saddr.sin_addr), newfd);
                       
                        strcpy(ip_buf1,inet_ntoa(saddr.sin_addr)); 
                       // printf("%s",ip_buf1);
                       // send(newfd, ip_buf1, 30, 0);
                       
                    }
                }else{      //如果是已经建立的链接 If the connection has existed.
                    sprintf(buf, "From %2d:\n", i);
                    if((len = recv(i, buf + 9, BUF_SIZE - 10, 0)) <= 0){
						//链接断开或出错 Connection is broken or eror
                        if(len == 0){
                            printf("SocketID %d has left.\n", i);
                        }else{
                            perror("The recv() got end");
                        }
                        close(i);
                        FD_CLR(i, &master); //移除 Remove
                    }else{
						//处理得到的数据 Process the current data
                        len += 9;
                        buf[len] = '\0';
                        printf("%s\n", buf);
                        for(j = 0; j <= fdmax; j++){
                            if(FD_ISSET(j, &master) && j != listen_fd && j != i){
								//信号j在master列表里有，发给除了发信号的和监听的所有进程
								//If signal j exists in the Master port list, Send message to all process except itself.
                                if(send(j, buf, len, 0) == -1){
                                    perror("Send() error");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return (0);
}
