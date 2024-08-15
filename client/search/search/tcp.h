#ifndef TCP_H
#define TCP_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include<QSocketNotifier>
#include<string>
#include"json.hpp"
using Json = nlohmann::json;
//#pragma comment(lib, "ws2_32.lib")

class tcp : public QWidget
{
    Q_OBJECT
public:
    explicit tcp(QWidget *parent = nullptr);
    ~tcp();
    void sendmsg(Json &data);

    void start();
private slots:
     void recvmsg();
signals:
     void changeLabel(QString data);
     void web_changeLable(QString data);
private:
    int socfd;
    WSADATA wsaData;
    struct sockaddr_in addr;
    QSocketNotifier *notifier;
};

#endif // TCP_H
