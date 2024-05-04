#include"linuxheader.h"
#include <workflow/WFFacilities.h>
#include<wfrest/HttpServer.h>

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
    server.GET("/test",[](const wfrest::HttpReq *req,wfrest::HttpResp*resp)
               {
                    resp->String("hello");
               });

    server.POST("/test",[](const wfrest::HttpReq *req,wfrest::HttpResp*resp)
               {
                    resp->String("hello");
               });
    if(server.track().start(1234) == 0)
    {
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
