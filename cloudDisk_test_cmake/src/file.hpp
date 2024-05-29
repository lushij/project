#pragma once
#include <string>
#include<iostream>
#include <wfrest/HttpServer.h>
#include "wfrest/PathUtil.h"
#include <workflow/WFFacilities.h>
#include <workflow/MySQLResult.h>
#include <workflow/MySQLMessage.h>
#include <wfrest/json.hpp>
#include "token.hpp"
#include <functional>
#include <alibabacloud/oss/OssClient.h>
#include "sql.hpp"
#include<nlohmann/json.hpp>
#include "oss.hpp"
#include<SimpleAmqpClient/SimpleAmqpClient.h>
#include"rabbitmq.hpp"
using std::cout;
using std::endl;
using std::string;
using namespace AlibabaCloud::OSS;
using Json=nlohmann::json;
//检验token，发送页面
struct file_usr_info
{
    string _username;
    string _token;
    const wfrest::HttpReq*_req;
    wfrest::HttpResp*_resp;
    SeriesWork*_series;
    bool _isGETupload;//默认为true
    string _sha1;
    string _filepath;
    string _file_content;
    long long _filesize;
    string filename;
    Config _config;
    file_usr_info(string username,string token,wfrest::HttpResp*resp,bool isGETupload)
        :_username(username)
         ,_token(token)
         ,_resp(resp)
         ,_isGETupload(isGETupload)
    {}
    file_usr_info(string username,string token,const wfrest::HttpReq*req,wfrest::HttpResp*resp,bool isGETupload)
        :_username(username)
         ,_token(token)
         ,_req(req)
         ,_resp(resp)
         ,_isGETupload(isGETupload)
    {}
    

};


class file
{
public:
    file(string username,string token,wfrest::HttpResp*resp,bool isGETupload=true)  
        :_fusr( username, token,resp,isGETupload)
    {}
    file(string username,string token,const wfrest::HttpReq*req,wfrest::HttpResp*resp,bool isGETupload=true)  
        :_fusr( username, token,req,resp,isGETupload)
    {}
    void setSha1(string &sha1)
    {
        _fusr._sha1=sha1;
    }

    void setFileInfo(string &filepath,string & file_content,long long size)
    {
        _fusr._filepath=std::move(filepath);
        _fusr._file_content=std::move(file_content);
        _fusr._filesize=std::move(size);
    }
    ~file() 
    {
        if(_fusr._req) delete _fusr._req;
        if(_fusr._resp) delete _fusr._resp;
    }
    //token鉴权
    bool token_ack(string &redis_token)
    {
        if(_fusr._token.size() > 40) return false;//快速鉴权
        if(redis_token != _fusr._token) return false;
        return true;
    }
    void read_redis(WFRedisTask*redisTask)
    {
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
            /* _fusr._resp->File("static/view/testSha.html"); */
            if(value.is_error())
            {
                fprintf(stderr,"redis error\n");
                state = WFT_STATE_TASK_ERROR;
            }
            break;
        }

        if (state != WFT_STATE_SUCCESS){
            _fusr._resp->String("token not exict");
            fprintf(stderr, "Failed\n");
            return;
        }
        else{
            fprintf(stderr, "Success!\n");
        }

        std::string cmd;
        req->get_command(cmd);
        fprintf(stderr,"redis request, cmd = %s\n", cmd.c_str());
        if(value.is_nil())
        {
            //表示查询结果为nil，表示不存在
            _fusr._resp->String("token not exict");
            return;
        }
        else
        {
            if(_fusr._isGETupload)//表示为get
                _fusr._resp->File("../static/view/testSha.html");
            else
            {
                if(value.is_string()){
                    fprintf(stderr,"value is a string, value = %s\n", value.string_value().c_str());
                    char name[128]={0};
                    sprintf(name,"%s",value.string_view()->c_str());
                    _fusr._username=string(name);
                    fprintf(stderr,"username = %s\n", _fusr._username.c_str());
                }
                else if(value.is_array()){
                    fprintf(stderr,"value is string array\n");
                    for(size_t i = 0; i < value.arr_size(); ++i){
                        fprintf(stderr,"value at %lu = %s\n", i, value.arr_at(i).string_value().c_str());
                    } 
                }
                //先查询mysql全局文件表看一下是否该文件已存在，已存在直接返回成功就行
                //创建任务
                //保存文件内容，写入数据库相关信息
                 using Form = std::map<std::string, std::pair<std::string, std::string>>;
                 Form &form = _fusr._req->form();
                 std::pair<std::string, std::string> fileInfo = form["file"];
                 std::pair<std::string, std::string> fileSha1 = form[fileInfo.first];
                 std::string sha1 = fileSha1.second.c_str();
                 fprintf(stderr,"fileSha1 = %s\n",fileSha1.second.c_str());//文件名对应的哈希
                 this->setSha1(sha1);
                 sql sqls;
                 fprintf(stderr,"sqls = %s\n",sqls.select_tbl_file_sqls(sha1).c_str());
                 // fileInfo.first fileInfo.second
                 std::string filepath = "../tmp/" + fileInfo.first;
                 this->setfile_name(fileInfo.first);
                 this->setFileInfo(filepath,fileInfo.second,fileInfo.second.size());
                 auto selectSha1 = WFTaskFactory::create_mysql_task(sqls.geturl(),0,[this](WFMySQLTask*mysqlTask){this->readMysql(mysqlTask);});
                 //设置属性
                 selectSha1->get_req()->set_query(sqls.select_tbl_file_sqls(sha1));
                 series_of(redisTask)->push_back(selectSha1);
            }
        }
    }

    void readMysql(WFMySQLTask*readTask)
    {
        //提取mysql中readTask的结果
        auto resp = readTask->get_resp();
        protocol::MySQLResultCursor cursor(resp);
        std::vector<std::vector<protocol::MySQLCell>> rows;
        cursor.fetch_all(rows);
        /* fprintf(stderr,"%s\n%s\n",_fusr._filepath.c_str(),this->_fusr._file_content.c_str()); */
        if(rows.size()==0)
        {
            //如果没有，插入数据库，保存文件
            /* _fusr._resp->Save(this->_fusr._filepath,std::move(this->_fusr._file_content)); */
            int fd = open(this->_fusr._filepath.c_str(),O_CREAT | O_RDWR,0664);
            if(fd < 0)
            {
                return ;
            }
            int ret = write(fd,this->_fusr._file_content.c_str(),this->_fusr._filesize);
            if(ret == this->_fusr._filesize)
            {
                this->_fusr._resp->set_status(200);
            }
            close(fd);
            //
            string filename = wfrest::PathUtil::base(this->_fusr._filepath);
            string sqls = sql().insert_tbl_file_sqls(this->_fusr._sha1,filename,this->_fusr._filesize,this->_fusr._filepath,0);
            string usr_sqls = sql().insert_tbl_usr_file_sqls(this->_fusr._username,this->_fusr._sha1,filename,this->_fusr._filesize);
            /* cout<<sqls<<endl; */
            /* cout<<usr_sqls<<endl; */
            _fusr._resp->MySQL(sql().geturl(),sqls);
            _fusr._resp->MySQL(sql().geturl(),usr_sqls);
#if 1
            if(_fusr._config.storeType==OSS)
            {
                OSSInfo info;
                ClientConfiguration conf;
                OssClient client(info.Endpoint,info.AccessKeyId,info.AccessKeySecret, conf);
                std::string osspath = "disk/"+_fusr._sha1;
                fprintf(stderr,"filepath = %s\nosspath = %s\n",_fusr._filepath.c_str(),osspath.c_str());
                if(_fusr._config.isAsyncTransferEnable == false){
                    auto outcome = client.PutObject(info.Bucket,osspath,_fusr._filepath);//把本地的filepath存入OSS的osspath
                    if(!outcome.isSuccess()){
                        fprintf(stderr,"PutObject fail, code = %s, message = %s, request id = %s\n", outcome.error().Code().c_str()
                                ,outcome.error().Message().c_str()
                                ,outcome.error().RequestId().c_str());
                    }
                }
                else
                {
                    RabbitMqInfo mqInfo;
                    AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create();
                    Json uploadInfo;
                    uploadInfo["filepath"]=_fusr._filepath;
                    uploadInfo["osspath"]=osspath;
                    /* uploadInfo["filsSha1"]=_fusr._sha1; */
                    AmqpClient::BasicMessage::ptr_t message = AmqpClient::BasicMessage::Create(uploadInfo.dump());
                    channel->BasicPublish(mqInfo.TransExchangeName,mqInfo.TransRoutingKey,message);
                }
            }
#endif
        }
        else
        {
            //如果有直接返回成功200
            this->_fusr._resp->set_status(200);
            //插入数据库个人信息表
            string filename = wfrest::PathUtil::base(this->_fusr._filepath);
            string sqls = sql().insert_tbl_file_sqls(this->_fusr._sha1,filename,this->_fusr._filesize,this->_fusr._filepath,0);
            string usr_sqls = sql().insert_tbl_usr_file_sqls(this->_fusr._username,this->_fusr._sha1,filename,this->_fusr._filesize);
            _fusr._resp->MySQL(sql().geturl(),sqls);
            _fusr._resp->MySQL(sql().geturl(),usr_sqls);
            return;
        }

    }
    void setConfig(const Config &_config)
    {
        _fusr._config=_config;
    }
    void setfile_name(const string&filename)
    {
        _fusr.filename=filename;
    }
private:
    file_usr_info _fusr;
};

