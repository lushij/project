#include "tcp.h"
#include<QDebug>
#include<string>
using std::string;
tcp::tcp(QWidget *parent) : QWidget(parent)
{
    qDebug()<<1;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    socfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(socfd < 0)
    {
        qDebug()<<"socket error";
        WSACleanup();
    }
    addr.sin_port=htons(8888);
    addr.sin_family=AF_INET;
    addr.sin_addr.S_un.S_addr= inet_addr("192.168.1.128");
     //addr.sin_addr.S_un.S_addr= inet_addr("localhost");
    SOCKET ret = ::connect(socfd,(struct sockaddr*)&addr,sizeof (addr));
    if(ret == (SOCKET)SOCKET_ERROR)
    {
        closesocket(socfd);
        WSACleanup();
        return;
    }
    qDebug()<<"connected!";
    // 创建QSocketNotifier来监控socfd的可读事件
    notifier = new QSocketNotifier(socfd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated, this, &tcp::recvmsg);
}

tcp::~tcp()
{
    closesocket(socfd);
    WSACleanup();
}

void tcp::sendmsg(Json &data)
{
    // 检查连接是否有效
    if (socfd == (int)INVALID_SOCKET)
    {
        qDebug() << "Invalid socket";
        return;
    }
    // 将JSON消息转换为字符串
    std::string jsonString = data.dump();
    jsonString+="\n";
    int ret = send(socfd,jsonString.c_str(),jsonString.size(),0);
    if(ret < 0)
    {
        closesocket(socfd);
        WSACleanup();
        return;
    }
    qDebug()<<"send "<<ret;
    qDebug() << "JSON message:" << jsonString.c_str();
}


void tcp::recvmsg()
{
    u_long bytesAvailable = 0;
    // 获取套接字缓冲区中可读取数据的大小
    //底层原理
    /*
        ioctlsocket 函数使用 FIONREAD 命令来查询接收缓冲区中的数据量。
        这是通过操作系统提供的网络栈实现的。
        网络栈会维护每个套接字的状态，包括接收缓冲区中已到达但未被应用程序读取的数据量。
    */
    if (ioctlsocket(socfd, FIONREAD, &bytesAvailable) == SOCKET_ERROR)
    {
        qDebug() << "获取缓冲区大小失败";
        closesocket(socfd);
        WSACleanup();
        return;
    }
    if (bytesAvailable == 0)
    {
        qDebug() << "没有数据可读";
        exit(-1);
    }

    std::string buffer;
    buffer.reserve(bytesAvailable); // 预先分配内存以减少重新分配的开销
    while (bytesAvailable > 0)
    {
        char tempBuf[4096];
        int ret = recv(socfd, tempBuf, sizeof(tempBuf), 0);
        if (ret <= 0)
        {
            if (ret == 0)
            {
                qDebug() << "连接关闭";
            }
            else
            {
                qDebug() << "接收消息错误";
            }
            closesocket(socfd);
            WSACleanup();
            return;
        }

        buffer.append(tempBuf, ret);
        bytesAvailable -= ret;
    }

    if(!buffer.empty())
    {
         //qDebug() << "接收消息" << buffer.c_str();
        if(buffer.find("KEY") != std::string::npos)
        {
            //功能1
            qDebug() << "KEY 功能1";
            // 发送信号给主页面
            emit changeLabel(QString::fromStdString(buffer));
        }
        else
        {
            //功能2
            qDebug() << "WEB 功能2";
            emit web_changeLable(QString::fromStdString(buffer));
        }
    }


}

