#include "tcpsocket.h"
#include<QAbstractSocket>
Tcpsocket::Tcpsocket(QWidget *parent) : QWidget(parent)
{
    dialog = new QDialog(this);
    dialog->resize(300,200);
    connect(&_socket,&QTcpSocket::disconnected,[&](){
        dialog->setWindowTitle("Not connected!");
        dialog->exec();
       });
    start();
}


