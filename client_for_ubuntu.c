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




#define  IN_A 1
#define  IN_B 2
#define  IN_C 3
#define  IN_D 4
#define  COM_SPEED  0

#define SERVER_PORT 5500
#define BUF_SIZE 512

int socketfd;
int servo1;
int servo2;



void backward(){
 
}
void forward(){
 
  
}
void turn_left(){
 
 
}
void turn_right(){
 
 
}

void car_stop(){
 
  

}

/* Suspend execution for x milliseconds intervals.
 *  @param ms Milliseconds to sleep.
 */

void delayMS(int x)
{
  usleep(x * 1000);
}
void car_com(char *buffer)
{int i;
 

 if(strlen(buffer)>=4){
 for( i=0;buffer[i+3]!='\0';i++){
 if(buffer[i]=='c'&&buffer[i+1]=='a'&&buffer[i+2]=='o'){
    switch (buffer[i+3]){
  case 'w':
    forward();
    strcpy(buffer,"forward");
    break;
  case 's':
    backward();
    strcpy(buffer,"backward");
    break;
  case 'a':
    turn_left();
    strcpy(buffer,"turn_left");
    break;
  case 'd':
    turn_right();
    strcpy(buffer," turn_right");
    break;
  case '1':
  
    strcpy(buffer,"speed1");   
    break;
  case '2':
    
    strcpy(buffer,"speed2");   
    break;
  case '3':
   
    strcpy(buffer,"speed3");
    break;
  case '4':
   
    strcpy(buffer,"speed4");
    break;
  case '5':
  
    strcpy(buffer,"speed5");
    break;
  case 'k':
    if(servo1<=1250)servo1=100+servo1;
    printf("%d",servo1);
    strcpy(buffer,"k");
    break;
  case 'i':
   if(servo1>=-250)servo1=servo1-100;
    printf("%d",servo1);
    strcpy(buffer,"i");
    break;
  case 'j':
    if(servo2<=1250)servo2+=100;
    printf("%d",servo2);
    strcpy(buffer,"j");
    break;
  case 'l':
     if(servo2>=-250)servo2-=100;
    printf("%d",servo2);
    strcpy(buffer,"l");
    break;
  }
}
}
}
}


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
    servo1=0;
    servo2=0;
    printf("%d,%d",servo1,servo2);
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
            car_com(buf);
            printf("%s\n", buf);
        }
    }
    close(socketfd);
    return (0);
}
