#include "SearchEngine.h"
#include "TcpConnection.h"

#include <iostream>


using std::cout;
using std::endl;
using json = nlohmann::json;
MyTask::MyTask(const string &msg, const TcpConnectionPtr &con)
: _msg(msg)
, _con(con)
{

}
void MyTask::process()
{
    json data = json::parse(_msg);
    cout << ">>task msg = " <<data.dump()<<"\n";
    json _sendmsg;
    switch (int(data["chose"]))
    {
    case KEY:
        {
            Keyword keyword(data["text"]);
            cout << " case KEY "<<"\n";
            _sendmsg = keyword.get_send_msg();
        }
        break;
    case WEB:
        {
            cout << " case WEB "<<"\n";
            cout << data["text"] <<"\n";
            queryWeb querweb(data["text"]);
            _sendmsg = querweb.get_send_msg();
            // try {
            //     std::cout << "Before creating queryWeb object" << "\n";
            //     queryWeb querweb(data["text"]);
            //     std::cout << "After creating queryWeb object" << "\n";
            //     _sendmsg = querweb.get_send_msg();
            // } catch (const std::bad_alloc &e) {
            //     std::cerr << "Memory allocation failed: " << e.what() << std::endl;
            // }
        }
        break;
    default:
        break;
    }
    cout << ">>send msg = " <<_sendmsg.dump()<<"\n";
    _con->sendInLoop(_sendmsg.dump());
}

SearchEngine::SearchEngine(size_t threadNum, size_t queSize
                       , const string &ip
                       , unsigned short port)
: _pool(threadNum, queSize)
, _server(ip, port)
{

}

SearchEngine::~SearchEngine()
{

}

void SearchEngine::start()
{
    _pool.start();
    using namespace  std::placeholders;
    _server.setAllCallback(std::bind(&SearchEngine::onNewConnection, this, _1)
                           , std::bind(&SearchEngine::onMessage, this, _1)
                           , std::bind(&SearchEngine::onClose, this, _1));
    _server.start();
}

void SearchEngine::stop()
{
    _pool.stop();
    _server.stop();
}

void SearchEngine::onNewConnection(const TcpConnectionPtr &con)
{
    cout << con->toString() << " has connected!" << endl;
}

void SearchEngine::onMessage(const TcpConnectionPtr &con)
{
    /* string msg = con->receive(); */
    /* cout << ">>recv client msg = " << msg << endl; */
    json data = json::parse(con->receive());
    cout << ">>recv client msg = " << data.dump() << endl;
    cout<<">>recv client chose = "<<data["chose"]<<'\n';

    //做业务逻辑的处理
    //在此处，需要将接受到的数据msg打包给线程池进行处理
    //
    MyTask task(data.dump(), con); 
    _pool.addTask(std::bind(&MyTask::process, task)); 
}

void SearchEngine::onClose(const TcpConnectionPtr &con)
{
    cout << con->toString() << " has closed!" << endl;
}
