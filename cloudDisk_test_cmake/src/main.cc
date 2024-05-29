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
#include "oss.hpp"
#include <alibabacloud/oss/OssClient.h>
#include "sendPhoneMsg.hpp"
#include "ConsistentHashing.hpp"
static WFFacilities::WaitGroup WaitGroup(1);
void sigHandler(int num)
{
    WaitGroup.done();
    fprintf(stderr,"wati group done\n");
}


void localFile(string &filename,string&strfilesize,wfrest:: HttpResp * resp)
{
        //方案一:
        string filepath = "../tmp/" + filename;
        int fd = open(filepath.c_str(), O_RDONLY);
        if(fd < 0) {
            perror("open");
            return;
        }
        size_t filesize = std::atoll(strfilesize.c_str());
        char * pbuff = new char[filesize + 1]();
        read(fd, pbuff, filesize);
        resp->append_output_body(pbuff, filesize);
        resp->headers["Content-Type"] = "application/octet-stream";
        resp->headers["Content-Disposition"] = "attachment;filename=" + filename;
        delete pbuff;
}

void OSS_FILE(string &filename,string & filesha1,wfrest::HttpResp*resp)
{
    string osspath="disk/"+filesha1;
    cout<<osspath<<endl;
    OSSInfo info;
    ClientConfiguration conf;
    OssClient client(info.Endpoint,info.AccessKeyId,info.AccessKeySecret,conf);
    GetObjectRequest req(info.Bucket,osspath);
    time_t t = std::time(nullptr) + 1200;
    auto outcome = client.GeneratePresignedUrl(info.Bucket,osspath,t,Http::Get);
    if(outcome.isSuccess())
    {
        resp->headers["Location"]= outcome.result().c_str();
        resp->set_status(301);
        std::cout<<"download success"<<endl;
    }
    else
    {
        std::cout<<"fail"<<endl;
    }
}
int main()
{
    signal(SIGINT,sigHandler);
    AlibabaCloud::InitializeSdk();
    AlibabaCloud::OSS::InitializeSdk();
    wfrest::HttpServer server;
    Config config;
    server.GET("/",[](const wfrest::HttpReq *req,wfrest::HttpResp*resp)
               {
               resp->File("../static/view/signin.html"); 
    });

    server.GET("/usr/signup",[](const wfrest::HttpReq *req,wfrest::HttpResp*resp)
               {
               resp->File("../static/view/signup.html"); 
    });
    server.GET("/home.html",[](const wfrest::HttpReq *req, wfrest::HttpResp *resp){
        resp->File("../static/view/home.html");
    });
   server.GET("/static/js/auth.js",[](const wfrest::HttpReq *req, wfrest::HttpResp *resp){
        resp->File("../static/js/auth.js");
    });
    server.GET("/static/img/avatar.jpeg",[](const wfrest::HttpReq *req, wfrest::HttpResp *resp){
        resp->File("../static/img/avatar.jpeg");
    });
    server.GET("/file/upload/success", [](const wfrest::HttpReq * ,wfrest:: HttpResp * resp){
        /* resp->String("upload success"); */
        resp->File("../static/view/uploadSuccess.html");
    });
    server.GET("/file/upload",[](const wfrest::HttpReq *req, wfrest::HttpResp *resp,SeriesWork*series){
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
    });
    
    server.POST("/file/upload",[config](const wfrest::HttpReq *req, wfrest::HttpResp *resp,SeriesWork*series){
        //从请求头提取token
        string _token = token().extractToken(req->header("Authorization")); 
        fprintf(stderr,"%s\n",_token.c_str());
        //因为到这一步就说明token是有效的就不需要再验证了,这一步是为了从redis中根据token获取username
         //创建redis任务，进行查询获取username
         file *file_usr = new file("",_token,req,resp,false);
         file_usr->setConfig(config);
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
#if 1//测试先不开启
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
    server.POST("/user/info", [](const wfrest::HttpReq * req,wfrest:: HttpResp * resp){
        //1.解析请求
        auto queryList = req->query_list();
        string username = queryList["username"];
        string strToken = queryList["token"];
        cout << "username:" << username << endl;
        cout << "token:" << strToken << endl;
        //2. 对token进行校验
        if(strToken.size()!=40)
        {
            resp->String("token is not effective");
        }
        
        //3. 访问数据库
        using namespace protocol;
        resp->MySQL(sql().geturl(), sql().select_siguptime_sqls(username), [username, resp](MySQLResultCursor * pcursor){
            vector<vector<MySQLCell>> rows;
            pcursor->fetch_all(rows);
            string signUpTime = rows[0][0].as_datetime();
            Json msg;
            Json data;
            data["Username"] = username;
            data["SignupAt"] = signUpTime;
            msg["data"] = data;
            resp->String(msg.dump());
        });
    });
   server.POST("/file/query", [](const wfrest:: HttpReq * req,wfrest:: HttpResp * resp){
        //1. 解析请求
        auto queryList = req->query_list();
        string username = queryList["username"];
        auto formKV = req->form_kv();
        string strlimit = formKV["limit"];
        cout << "username:" << username << endl;
        cout << "limit:" << strlimit << endl;

        //2. 访问数据库MySQL，获取用户相关的文件信息
        using namespace protocol;
        resp->MySQL(sql().geturl(), sql().select_usr_tbl_file_sqls(username),[resp](MySQLResultCursor * pcursor){
            vector<vector<MySQLCell>> rows;
            pcursor->fetch_all(rows);
            Json msgArr;
            for(size_t i = 0; i < rows.size(); ++i) {
                Json row;
                row["FileHash"] = rows[i][0].as_string();
                row["FileSize"] = rows[i][1].as_ulonglong();
                row["FileName"] = rows[i][2].as_string();
                row["UploadAt"] = rows[i][3].as_datetime();
                row["LastUpdated"] = rows[i][4].as_datetime();
                msgArr.push_back(row);
            }
            cout << "msgArr:" << msgArr.dump() << endl;
            resp->String(msgArr.dump());
        });
    });
        server.GET("/file/download", [](const wfrest::HttpReq * req,wfrest:: HttpResp * resp){
        Node n1(1),n2(2),n3(3);//1.表示本地，2.nginx，3.OSS
        vector<Node*>servers_ip_chose = {&n1,&n2,&n3};
        LoadBalancer balancer;
        //1. 解析请求
        string filename = req->query("filename");
        string strfilesize = req->query("filesize");
        string sha1= req->query("filesha1");
        cout << "filename:" << filename << endl;
        cout << "filesize:" << strfilesize << endl;
        cout<<"sha1 = "<<sha1<<endl;
        int ret = balancer.simulateRequests(servers_ip_chose,sha1);
        //优化，加入负载均衡，本地，nginx，oss
        switch(ret)
        {
        case 1:
            cout<<"local file"<<endl;
            localFile(filename,strfilesize,resp);
        break;
        case 2:
            cout<<"nginx file"<<endl;
            //2. 重定向到另外一个下载服务器
            resp->headers["Location"] = "http://139.196.122.57:1235/" + filename;
            resp->set_status_code("301");
        break;
        case 3:
            cout<<"oss file"<<endl;
            OSS_FILE(filename,sha1,resp);
        break;
        default:
        break;
        }
       

        });

    server.POST("/file/downloadurl", [](const wfrest:: HttpReq * req,wfrest:: HttpResp * resp){
        //1.解析请求
        auto queryList = req->query_list();
        string filename = queryList["filename"];
        cout<<filename<<endl;
        string filesha1 = queryList["filehash"];
        string filesize = queryList["filesize"];
        cout<<"filesha1 = "<<filesha1<<endl;
        //2. 构造下载链接
        string downloadurl = "http://139.196.122.57:1236/file/download?filename=" + filename+"&filesha1="+filesha1+"&filesize="+filesize;
        resp->String(downloadurl);
    });
    if(server.track().start(1236) == 0)
    {
        server.list_routes();
        WaitGroup.wait();//使主线程陷入阻塞
        server.stop();
        // 调用 AlibabaCloud::OSS 命名空间下的 ShutdownSdk() 函数
        AlibabaCloud::OSS::ShutdownSdk();
        AlibabaCloud::ShutdownSdk();

    }
    else
    {
        fprintf(stderr,"can not start server\n");
        return -1;
    }
    return 0;
}
