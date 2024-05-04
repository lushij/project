#pragma once
#include "linuxheader.h"
#include <string>
#include<wfrest/HttpServer.h>
#include <workflow/WFFacilities.h>
#include <workflow/MySQLResult.h>
#include <workflow/MySQLMessage.h>
#include <wfrest/json.hpp>
using std::string;
class sql
{
public:
    sql() {}
    ~sql() {}
    //处理全局文件表
    string insert_tbl_file_sqls(const string &file_sha1,const string& file_name, long long & file_size,const string &file_addr,int status)
    {
        string sql="INSERT INTO "+ db + ".tbl_file (file_sha1,file_name,file_size,file_addr,status) VALUES("+
            "'"+file_sha1+"'," +
            "'"+file_name+"',"+
            "'"+std::to_string(file_size)+"',"+
            "'"+file_addr+"',"+
            "'"+std::to_string(status)+"');";
        return sql;
    }
    string insert_tbl_usr_file_sqls(const string & user_name,const string&file_sha1,const string&file_name,long long &file_size)
    {
        string sql="INSERT INTO "+db+".tbl_user_file (user_name,file_sha1,file_name,file_size) VALUES("+
            "'"+user_name+"',"+
            "'"+file_sha1+"',"+
            "'"+file_name+"',"+
            "'"+std::to_string(file_size)+"');";
        return sql;
    }
    string insert_tbl_usr_sqls(const string&user_name,const string&user_pwd ,const string&salt)
    {
        string sql = "INSERT INTO "+db+".tbl_user (user_name,user_pwd,salt) VALUES("+
            "'"+user_name+"',"+
            "'"+user_pwd+"',"+
            "'"+salt+"');";
        return sql;
    }
    string select_tbl_user_sqls(const string&username)
    {
        //根据用户名找盐值和密文
        string sql = "SELECT user_pwd,salt FROM "+db+".tbl_user WHERE user_name = '"+username+"' LIMIT 1;";
        return sql;
    }
    string select_tbl_file_sqls(const string&file_sha1)
    {
        string sql = "SELECT file_addr FROM "+db+".tbl_file WHERE file_sha1 = '"+file_sha1+"' LIMIT 1;";
        return sql;
    }
    string geturl()const
    {
        return url;
    }
private:
    static string db;//数据库名字
    static string url;//数据库地址
};
string sql::url="mysql://root:root123.@localhost";
string sql::db = "cloudDisk";
