#pragma once
#include <functional>
#include<string>
#include<wfrest/HttpServer.h>
#include <workflow/WFFacilities.h>
#include <workflow/MySQLResult.h>
#include <workflow/MySQLMessage.h>
#include <wfrest/json.hpp>
#include <iostream>
#include "Hash.hpp"
#include "Crypt.hpp"
#include "sql.hpp"
#include "token.hpp"
#include "phone_code.hpp"
#include "oss.hpp"
using std::endl;
using std::cout;
using Json=nlohmann::json;
static std::string redis_url="redis://127.0.0.1:6379";
static std::string mysql_url="";
using std::string;
enum VERIFY{
    OK=1,
    PASSWORD_ERR,
    CODE_ERR,
};
class usr
{
public:
    usr(const string&username,const string&password,string&sql_url,wfrest::HttpResp*_resp)
    :_username(username)
     ,_password(password)
     ,sqlurl(sql_url)
     ,resp(_resp)
    {}
    usr(const string &username,const string &password,const string& phone,const string &phoneCode) 
    :_username(username)
     ,_password(password)
     ,_phone(phone)
     ,_phoneCode(phoneCode)
    {
    }
    ~usr() {}
    void verify_code(WFRedisTask*redisTask)
    {
        cout<<_username<<"---"<<_password<<"---"<<_phone<<"---"<<_phoneCode<<endl;
        usr * info = static_cast<usr*>(series_of(redisTask)->get_context());
        protocol::RedisRequest *req = redisTask->get_req();
        protocol::RedisResponse *resp = redisTask->get_resp();
        int state = redisTask->get_state();
        int error = redisTask->get_error();
        protocol::RedisValue value;//value对象专门用来存储redis任务的结果
        switch (state)
        {
        case WFT_STATE_SYS_ERROR:
            fprintf(stderr,"system error: %s\n", strerror(error));
            break;
        case WFT_STATE_DNS_ERROR:
            fprintf(stderr,"dns error: %s\n", gai_strerror(error));
            break;
        case WFT_STATE_SUCCESS:
            resp->get_result(value);
            if(value.is_error())
            {
                fprintf(stderr,"redis error\n");
                state = WFT_STATE_TASK_ERROR;
            }
            break;
        }

        if (state != WFT_STATE_SUCCESS){
            fprintf(stderr, "Failed\n");
            return;
        }
        else{
            fprintf(stderr, "Success!\n");
        }

        std::string cmd;
        req->get_command(cmd);
        fprintf(stderr,"redis request, cmd = %s\n", cmd.c_str());
        if(value.is_string()){
            fprintf(stderr,"value is a string, value = %s\n", value.string_value().c_str());
            fprintf(stderr,"this_code = %s\n",_phoneCode.c_str());
            if(_phoneCode==value.string_value())
            {
                //表示验证码正确
                /* info->resp->append_output_body("SUCCESS",7); */
                Crypt crypt(_password);//加密密码
                string salt = crypt.getsalt();//盐值
                char *_cryptPassword=crypt.getcrypt_password();//密文
                fprintf(stderr,"盐值=%s \n密文=%s\n",salt.c_str(),_cryptPassword);
                sql mysql;
                string mysqls = mysql.insert_tbl_usr_sqls(_username,_cryptPassword,salt);
                fprintf(stderr,"sql = %s\n",mysqls.c_str());
                auto mysqlTask = WFTaskFactory::create_mysql_task(mysql.geturl(),0,[this](WFMySQLTask*mysqlTask){this->write_sql(mysqlTask);});
                mysqlTask->get_req()->set_query(mysqls);
                series_of(redisTask)->push_back(mysqlTask); 
                mysqlTask->user_data=info;
                //在mysql任务重进行回复，因为可以判断是否注册过
            }   
            else
            {
                info->resp->append_output_body("CODE_FAIL",9);
                return;
            }
        }
        else if(value.is_array()){
            fprintf(stderr,"value is string array\n");
            for(size_t i = 0; i < value.arr_size(); ++i){
                fprintf(stderr,"value at %lu = %s\n", i, value.arr_at(i).string_value().c_str());
            } 
        }
    }
    void write_sql(WFMySQLTask*mysqlTask)
    {
        usr * info = static_cast<usr*>(mysqlTask->user_data);
        fprintf(stderr,"%s\n",info->_username.c_str());
        if(mysqlTask->get_state() != WFT_STATE_SUCCESS){
            fprintf(stderr,"error msg:%s\n",WFGlobal::get_error_string(mysqlTask->get_state(), mysqlTask->get_error()));
            info->resp->append_output_body("username_repeat",15);
            return;
        }

        protocol::MySQLResponse *resp = mysqlTask->get_resp();
        protocol::MySQLResultCursor cursor(resp);

        // 检查语法错误
        if(resp->get_packet_type() == MYSQL_PACKET_ERROR){
            info->resp->append_output_body("username_repeat",15);
            fprintf(stderr,"error_code = %d msg = %s\n",resp->get_error_code(), resp->get_error_msg().c_str());
            return;
        }

        if(cursor.get_cursor_status() == MYSQL_STATUS_OK){
            //写指令，执行成功
            fprintf(stderr,"OK. %llu rows affected. %d warnings. insert_id = %llu.\n",
                    cursor.get_affected_rows(), cursor.get_warnings(), cursor.get_insert_id());
            
            if(cursor.get_affected_rows() == 1){
                info->resp->append_output_body("SUCCESS",7);
                return;
            }
        }
    }

    void read_mysql(WFMySQLTask*mysqlTask)
    {
        usr*info=static_cast<usr*>(series_of(mysqlTask)->get_context());
        //检验密码
        //查数据库获取盐值
        //重新算加密密码然后核对数据库中的加密密文，一致返回token
        //提取request的结果
        auto resp = mysqlTask->get_resp();
        protocol::MySQLResultCursor cursor(resp);//迭代器便于遍历
        std::vector<std::vector<protocol::MySQLCell>>rows;
        //把结果保存在rows中
        cursor.fetch_all(rows);
        std::string sqlPassword = rows[0][0].as_string();
        std::string salt = rows[0][1].as_string();
        //根据用户输入的密码重新原方法加密与数据库中的密文进行比对
        std::string newPassword=Crypt().virpty(_password,salt);
        fprintf(stderr,"数据库的密码为%s,salt = %s\n",sqlPassword.c_str(),salt.c_str());
        fprintf(stderr,"新密码为%s\n",newPassword.c_str());
        if(newPassword==sqlPassword)
        {
            //生成token存入reides，并设置expire时间1天
            info-> _token = token(_username)._token;
            fprintf(stderr,"token = %s\n",info->_token.c_str());
            fprintf(stderr,"class token = %s\n",_token.c_str());
            //创建redis写任务
            phone_code tokenInfo(info->_token);
            std::function<void (WFRedisTask*)> func = [&tokenInfo](WFRedisTask*redisTask){tokenInfo.write_redis(redisTask);};
            auto writeRedis = WFTaskFactory::create_redis_task(redis_url,0,func);
            //设置属性
            protocol::RedisRequest*redis_req = writeRedis->get_req();
            /* redis_req->set_request("SET",{info->_token,tokenInfo.get_code()}); */
            redis_req->set_request("SET",{info->_token,_username});//token  username
            //创建expire任务
            auto ExpireRedis = WFTaskFactory::create_redis_task(redis_url,0,func);
            protocol::RedisRequest*expire_req = ExpireRedis->get_req();
            expire_req->set_request("EXPIRE",{info->_token,"86400"});
            //加入序列
            series_of(mysqlTask)->push_back(writeRedis);
            series_of(mysqlTask)->push_back(ExpireRedis);
            //并返回token
            Json uinfo;
            uinfo["Username"] = info->_username;
            uinfo["Token"] = info->_token;
            uinfo["Location"] = "/home.html";
            Json pInfo;
            pInfo["code"] = 0;
            pInfo["msg"] = "OK";
            pInfo["data"] = uinfo;
            info->resp->String(pInfo.dump());
        }
        else
        {
            info->resp->append_output_body("FAIL",4);
            return;
        }
    }
    void setConfig(Config &config)
    {
        _config=config;
    }
public:
    const string _username;
    const string _password;
    const string _phone;
    const string _phoneCode;
    VERIFY _varify;
    const wfrest::HttpReq *req;
    wfrest::HttpResp*resp;
    string _iscodeok;
    const char*_crypt_passsword;
    string sqlurl;
    string _token;
    Config _config;
};

