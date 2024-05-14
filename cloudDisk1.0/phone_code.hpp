#pragma once
#include<string>
#include<wfrest/HttpServer.h>
#include <workflow/WFFacilities.h>
#include <workflow/MySQLResult.h>
#include <workflow/MySQLMessage.h>
#include <wfrest/json.hpp>
using std::string;
class phone_code
{
public:
    phone_code(string &phone,string &code)
        :_phone(phone)
         ,_code(code)
    {}
    phone_code(const string &token)
        :_phone(token)
         ,_code("1")
    {

    }
    ~phone_code() {}
    void write_redis(WFRedisTask *redisTask)
    {
        protocol::RedisRequest *req = redisTask->get_req();
        protocol::RedisResponse *resp = redisTask->get_resp();
        int state = redisTask->get_state();
        int error = redisTask->get_error();
        protocol::RedisValue value;//value对象专门用来存储redis任务的结果
        switch (state){
        case WFT_STATE_SYS_ERROR:
            fprintf(stderr,"system error: %s\n", strerror(error));
            break;
        case WFT_STATE_DNS_ERROR:
            fprintf(stderr,"dns error: %s\n", gai_strerror(error));
            break;
        case WFT_STATE_SUCCESS:
            resp->get_result(value);
            if(value.is_error()){
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
        }
        else if(value.is_array()){
            fprintf(stderr,"value is string array\n");
            for(size_t i = 0; i < value.arr_size(); ++i){
                fprintf(stderr,"value at %lu = %s\n", i, value.arr_at(i).string_value().c_str());
            } 
        }

    }        
    string get_token()const 
    {
        return  _phone;
    }
    string get_code()const
    {
        return _code;
    }
private:
string _phone;
string _code;
};

