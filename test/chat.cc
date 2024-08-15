#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <errno.h>


#define ARGS_CHECK(argc,num) {if(argc!=num){fprintf(stderr,"args error!\n");return -1;}}

#define ERROR_CHECK(ret,num,msg) {if(ret == num){perror(msg); return -1;}}
typedef struct Conn_s {
    int isConnected; // 0 未连接 1 已经连接
    int netfd;
    time_t lastActive;
} Conn_t;
int main(int argc, char *argv[])
{
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi("8888"));
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    int reuse = 1; //允许重用
    int ret = setsockopt(sockfd,SOL_SOCKET,
                         SO_REUSEADDR,&reuse,sizeof(reuse));
    ERROR_CHECK(ret,-1,"setsockopt");
    ret = bind(sockfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr));
    ERROR_CHECK(ret,-1,"bind");
    listen(sockfd,50);

    fd_set monitorSet;
    fd_set readySet;
    FD_ZERO(&monitorSet);
    FD_SET(sockfd,&monitorSet);
    // 设计一个数据结构 存储所有客户端的netfd和上次活跃时间
    Conn_t client[1024];
    for(int i = 0; i < 1024; ++i){
        client[i].isConnected = 0;
    }
    int curidx = 0; // 下一次加入的netfd的下标
    // 用于查找的哈希表 netfd --> idx
    int fdToidx[1024];
    for(int i = 0; i < 1024; ++i){
        fdToidx[i] = -1;
    }
    time_t now;
    char buf[4096];
    while(1){
        memcpy(&readySet,&monitorSet,sizeof(fd_set));
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        select(1024,&readySet,NULL,NULL,&timeout);
        now = time(NULL);
        printf("now = %s\n", ctime(&now));
        if(FD_ISSET(sockfd,&readySet)){ // 有新的客户端建立连接
            // accept创建新的netfd，将其信息插入到client
            client[curidx].isConnected = 1;
            client[curidx].netfd = accept(sockfd,NULL,NULL); // accept
            client[curidx].lastActive = time(NULL);
            printf("i = %d, netfd = %d, now = %s\n", curidx, client[curidx].netfd, ctime(&client[curidx].lastActive));
            fdToidx[client[curidx].netfd] = curidx;
            // 加入到监听结合
            FD_SET(client[curidx].netfd,&monitorSet);
            ++curidx;
        }
        // 在select返回之后，只能遍历所有的netfd，看是否就绪
        for(int i = 0; i < curidx; ++i){ // i用来遍历client
            if(client[i].isConnected == 1 && FD_ISSET(client[i].netfd, &readySet)){
                // 在线 & 发了消息
                bzero(buf,4096);
                ssize_t sret = recv(client[i].netfd,buf,sizeof(buf),0);
                if(sret == 0){
                    // 某个客户端断开连接
                    printf("%d 断开连接",client[i].netfd);
                    FD_CLR(client[i].netfd,&monitorSet);
                    close(client[i].netfd);
                    fdToidx[client[i].netfd] = -1;
                    client[i].isConnected = 0;
                    continue;
                }
                client[i].lastActive = time(NULL);
                printf("i = %d, netfd = %d, now = %s\n", i, client[i].netfd, ctime(&client[i].lastActive));
                // 转发
                for(int j = 0; j < curidx; ++j){ // j也是用来遍历client
                    if(client[j].isConnected == 1 && j != i){ // 在线 & 不是自己
                        send(client[j].netfd,buf,strlen(buf),0);
                    }
                }
            }
        }
        for(int i = 0; i < curidx; ++i){
            if(client[i].isConnected == 1 && now - client[i].lastActive > 5){
               FD_CLR(client[i].netfd,&monitorSet);
               close(client[i].netfd);
               fdToidx[client[i].netfd] = -1;
               client[i].isConnected = 0;
            }
        }
    }
    return 0;
}
