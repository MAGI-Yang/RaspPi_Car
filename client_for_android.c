//client.c
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

int socketfd;

void quit(int signo){
    close(socketfd);
    printf("Bye~Bye!\n");
    exit(0);
}

int main()
{
    struct sockaddr_in servaddr;
    char buf[BUF_SIZE];
    int type;
    pid_t pid;

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    type = connect(socketfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    if(type == -1){
        perror("There is an Error in your connnection! ");
        return -1;
    }
    printf("You can use \"Ctrl + D\" to quit.\n");

    if(signal(SIGUSR1, quit) == SIG_ERR){   //为信号SGUSR1设定信号处理函数
        perror("Can\'t set SIGUSR1 signal action");
        exit(1);
    }

    if((pid = fork()) != 0){
        while(fgets(buf, BUF_SIZE, stdin) != NULL)
			//父进程，处理读标准输入，并发数据 Parent process, Get input and send the messages.
        {           write(socketfd, buf, strlen(buf) - 1);
        }
        if(kill(pid, SIGUSR1) < 0)  //结束子进程 End of child process
        {
            perror("Kill error");
        }
    }else{
        while((type = read(socketfd, buf, BUF_SIZE - 1)) > 0){  //子进程，接收数据，并输出 child process
            buf[type] = '\0';
            printf("%s\n", buf);
        }
    }
    close(socketfd);
    return (0);
}