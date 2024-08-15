#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>


int main(int argc,char* argv[]){
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serAddr;
    memset(&serAddr,0,sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = INADDR_ANY;
    serAddr.sin_port = htons(8888);
    int ret = bind(sfd,(struct sockaddr*)&serAddr,sizeof(serAddr));
    ret = listen(sfd,10);
    int newFd = accept(sfd,NULL,NULL);
    char buf[64]={0};
    fd_set rdset;
    while(1){
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO,&rdset);
        FD_SET(newFd,&rdset);
        select(newFd+1,&rdset,NULL,NULL,NULL);
        if(FD_ISSET(STDIN_FILENO,&rdset)){
            memset(buf,0,sizeof(buf));
            int ret = read(STDIN_FILENO,buf,sizeof(buf));
            if(ret == 0){
                break;
            }
            send(newFd,buf,strlen(buf)-1,0);
        }
        else if(FD_ISSET(newFd,&rdset)){
            memset(buf,0,sizeof(buf));
            ret = recv(newFd,buf,sizeof(buf),0);
            //对端断开的时候，newFd一直可读
            //recv读数据的返回值是0
            if(0 == ret){
                printf("byebye\n");
                close(sfd);
                close(newFd);
                return 0;
            }
            printf("buf=%s\n",buf);
        }
    }
    close(newFd);
    close(sfd);
    return 0;
}
