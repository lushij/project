#include "linuxheader.h"
#include <wfrest/HttpServer.h>
#include <workflow/WFFacilities.h>
#include <workflow/MySQLResult.h>
#include <workflow/MySQLMessage.h>
#include <wfrest/json.hpp>
#include "token.hpp"
#include <functional>
#include "phone_code.hpp"
#include <openssl/aes.h>
#include <openssl/rand.h>
#include "usr.hpp"
#include "file.hpp"
#include <iostream>
/* #include"log.hpp" */
#include "sendPhoneMsg.hpp"
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
    server.GET("/home.html",[](const wfrest::HttpReq *req, wfrest::HttpResp *resp){
        resp->File("static/view/home.html");
    });
   server.GET("/static/js/auth.js",[](const wfrest::HttpReq *req, wfrest::HttpResp *resp){
        resp->File("static/js/auth.js");
    });
    server.GET("/static/img/avatar.jpeg",[](const wfrest::HttpReq *req, wfrest::HttpResp *resp){
        resp->File("static/img/avatar.jpeg");
    });
    server.GET("/file/upload",[](const wfrest::HttpReq *req, wfrest::HttpResp *resp,SeriesWork*series){
        /* resp->File("static/view/index.html"); */
        /* resp->File("static/view/test.html"); */
        /* resp->File("static/view/test6.html"); */
        /* resp->File("static/view/test8.html");//字样时间限制 */
        // 从URL中提取用户名的信息
        auto userInfo = req->query_list();
        std::string username = userInfo["username"];//在这里是"";
        std::string token = userInfo.begin()->first.c_str();//获取token
        fprintf(stderr,"前端发送的token = %s\n",token.c_str());
        if(token.size() != 40)
        {
            resp->String("Token not exicti from quick");
        }
        else
        {
            //创建redis任务，进行查询
            file *file_usr = new file(username,token,resp);
            auto func = [file_usr](WFRedisTask*redisTask){file_usr->read_redis(redisTask);};
            auto redisTask = WFTaskFactory::create_redis_task(redis_url,0,func);
            //设置属性
            protocol::RedisRequest*redisReq = redisTask->get_req();
            redisReq->set_request("GET",{token});
            series->push_back(redisTask);
            series->set_context(file_usr);
        }
        //判断token是否有效
        /* resp->File("static/view/testSha.html");//字样时间限制并加上sha值 */
        /* resp->File("static/view/test7.html");//进度条 */
    });
    
    server.POST("/file/upload",[](const wfrest::HttpReq *req, wfrest::HttpResp *resp,SeriesWork*series){
        //从请求头提取token
        string _token = token().extractToken(req->header("Authorization")); 
        fprintf(stderr,"%s\n",_token.c_str());
        //因为到这一步就说明token是有效的就不需要再验证了,这一步是为了从redis中根据token获取username
         //创建redis任务，进行查询获取username
         file *file_usr = new file("",_token,req,resp,false);
         auto func = [file_usr](WFRedisTask*redisTask){file_usr->read_redis(redisTask);};
         auto redisTask = WFTaskFactory::create_redis_task(redis_url,0,func);
         //设置属性
         protocol::RedisRequest*redisReq = redisTask->get_req();
         redisReq->set_request("GET",{_token});
         series->push_back(redisTask);
});




    server.POST("/usr/signin",[](const wfrest::HttpReq *req,wfrest::HttpResp*resp,SeriesWork*series)
                {
                //登录
                //1 按urlencoded的形式去解析post报文体
                std::map<std::string,std::string> &form_kv = req->form_kv();
                std::string username = form_kv["username"];
                std::string password = form_kv["password"];
                fprintf(stderr,"%s %s\n",username.c_str(),password.c_str());
                //检验密码
                //查数据库获取盐值
                //重新算加密密码然后核对数据库中的加密密文，一致返回token
                sql sqls;
                std::string sql_url =sqls.geturl();
                usr *info=new usr(username,password,sql_url,resp);
                auto mysqlTask = WFTaskFactory::create_mysql_task(sqls.geturl(),0,[info](WFMySQLTask*mysqlTask){info->read_mysql(mysqlTask);});
                //设置属性
                //先获取sql语句
                string sql_txt = sqls.select_tbl_user_sqls(username);
                fprintf(stderr,"sql = %s\n",sql_txt.c_str());
                mysqlTask->get_req()->set_query(sql_txt);
                series->set_context(info);
                series->push_back(mysqlTask);
                series->set_callback([](const SeriesWork*series){
                    delete static_cast<usr*>(series->get_context());
              });
   });



    //注册
    server.POST("/signup",[](const wfrest::HttpReq *req,wfrest::HttpResp*resp,SeriesWork*series){
                //从获取信息的手机号查redis中的验证码进行比对
                //注册
                auto from_kv=req->form_kv();
                std::string username=from_kv["username"];
                std::string password=from_kv["password"];
                std::string phone=from_kv["phone"];
                std::string phoneCode=from_kv["phoneCode"];
                /* fprintf(stderr,"-----------------------------\n"); */
                /* fprintf(stderr,"%s---%s---%s---%s\n",username.c_str(),password.c_str(),phone.c_str(),phoneCode.c_str()); */
                /* fprintf(stderr,"-----------------------------\n"); */
                usr* usrinfo=new usr(username,password,phone,phoneCode);
                auto redisTask = WFTaskFactory::create_redis_task(redis_url,0,[usrinfo](WFRedisTask*redis){usrinfo->verify_code(redis);});
                protocol::RedisRequest*redisreq=redisTask->get_req();
                redisreq->set_request("GET",{phone});
                series->push_back(redisTask);
                /* resp->String("SUCCESS"); */
                usrinfo->req=req;
                usrinfo->resp=resp;
                /* fprintf(stderr,"写入数据库\n"); */
                //成功加密密码
                series->set_context(usrinfo);
                series->set_callback([](const SeriesWork *series){
                    usr*info = static_cast<usr*>(series->get_context());
                    delete info;
                });
                
    });

    server.POST("/usr/getcode",[](const wfrest::HttpReq*req,wfrest::HttpResp*resp){
                //从uri获取
                auto from_kv=req->form_kv();
                fprintf(stderr,"%s\n",from_kv["phone"].c_str()); 
                std::string phone =from_kv["phone"] ;
                string code = phoneCode().get_code();
                sendPhoneMsg msgSender;
#if 0//测试先不开启
                if (!msgSender.send(phone, code)) {
                    return -1;
                }
#endif
                //把手机号和验证码插入redis,有效期为60s
                phone_code pcinfo(phone,code);
                //建立redis任务
                std::function<void(WFRedisTask*)> func = [&pcinfo](WFRedisTask* task) {pcinfo.write_redis(task);};
                auto redisTask = WFTaskFactory::create_redis_task(redis_url,0,func);
                //设置redis任务的属性
                protocol::RedisRequest*redisreq=redisTask->get_req();
                redisreq->set_request("SET",{phone,code});
                auto ExpireTask = WFTaskFactory::create_redis_task(redis_url,0,func);
                protocol::RedisRequest*expireReq=ExpireTask->get_req();
                expireReq->set_request("EXPIRE",{phone,"300"});
                redisTask->start();
                series_of(redisTask)->push_back(ExpireTask);
                fprintf(stderr,"code = %s\n",code.c_str());
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
