#include"linuxheader.h"
#include<wfrest/HttpServer.h>
#include <workflow/WFFacilities.h>
#include <workflow/MySQLResult.h>
#include <workflow/MySQLMessage.h>
#include <wfrest/json.hpp>
#include "token.hpp"
#include "oss.hpp"
#include "rabbitmq.hpp"
#include <SimpleAmqpClient/SimpleAmqpClient.h>
/* #include"log.hpp" */
using namespace AlibabaCloud::OSS;
static WFFacilities::WaitGroup WaitGroup(1);

void sigHandler(int num)
{
    WaitGroup.done();
    fprintf(stderr,"wati group done\n");
}

int main()
{
    signal(SIGINT,sigHandler);
    wfrest::HttpServer server;
    server.GET("/",[](const wfrest::HttpReq *req,wfrest::HttpResp*resp)
               {
                  resp->File("static/view/signin.html"); 
               });

    server.GET("/usr/signup",[](const wfrest::HttpReq *req,wfrest::HttpResp*resp)
               {
                  resp->File("static/view/signup.html"); 
               });
    server.POST("/usr/signin",[](const wfrest::HttpReq *req,wfrest::HttpResp*resp,SeriesWork*series)
               {
                    //1 按urlencoded的形式去解析post报文体
                     std::map<std::string,std::string> &form_kv = req->form_kv();
                     std::string username = form_kv["username"];
                     std::string password = form_kv["password"];
                    resp->String("HELLO");
                    auto usrinfo = req->query_list();
                    for(auto &v:form_kv)
                    {
                    std:: cout<<v.first<<" "<<v.second<<std::endl;
                    }
                    fprintf(stderr,"%s %s\n",username.c_str(),password.c_str());
               });
    //注册
    server.POST("/usr/signup",[](const wfrest::HttpReq*req,wfrest::HttpResp*resp,SeriesWork*series){
                    
                });
    server.POST("/usr/getcode",[](const wfrest::HttpReq*req,wfrest::HttpResp*resp){
                   
                });
    if(server.track().start(1236) == 0)
    {
        server.list_routes();
        WaitGroup.wait();//使主线程陷入阻塞
        server.stop();
    }
    else
    {
        fprintf(stderr,"can not start server\n");
        return -1;
    }
    return 0;
}
