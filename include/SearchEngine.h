#ifndef __SEARCHENGINE_H__
#define __SEARCHENGINE_H__ 

#include "ThreadPool.h"
#include "TcpServer.h"
#include "nlohmann/json.hpp"
#include "Keyword.h"
#include "queryWeb.h"
enum {
    KEY=1,
    WEB
};
class MyTask
{
public:
    MyTask(const string &msg, const TcpConnectionPtr &con);
    void process();
private:
    string _msg;
    TcpConnectionPtr _con;
};

class SearchEngine
{
public:
    SearchEngine (size_t threadNum, size_t queSize
               , const string &ip, 
               unsigned short port);
    ~SearchEngine ();

    void start();
    void stop();

    void onNewConnection(const TcpConnectionPtr &con);
    void onMessage(const TcpConnectionPtr &con);
    void onClose(const TcpConnectionPtr &con);

private:
    ThreadPool _pool;
    TcpServer _server;

};

#endif
