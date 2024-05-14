#pragma once
#include <string>
#include <openssl/md5.h>
#include "linuxheader.h"
#include "Crypt.hpp"
//token采用MD5加密 
class token
{
public:
    token(){}
    token(std::string usrname)
        :_usrname(usrname)
    {
       //用户名+salt用md5算法加密
       _salt=Crypt(usrname).getsalt();
       std::string tokenGen = usrname + _salt;
        unsigned char md[16];
            MD5((const unsigned char *)tokenGen.c_str(),tokenGen.size(),md);
            char frag[3] = {0};
            for(int i = 0;i < 16; ++i){
                sprintf(frag,"%02x",md[i]);
                _token = _token + frag;
            }

            char timeStamp[20];
            time_t now = time(NULL);
            struct tm *ptm = localtime(&now);
            sprintf(timeStamp,"%02d%02d%02d%02d",ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
            _token = _token + timeStamp;
    }
    ~token() {}

    std::string extractToken(const std::string& authHeader) 
    {
        // 检查Authorization头是否以Bearer开头
        if (authHeader.substr(0, 6) == "Bearer") {
            // 返回Bearer后面的部分
            return authHeader.substr(7);
        } else {
            // 如果Authorization头格式不正确，则返回空字符串
            return "";
        }
    }
public:
    std::string _token;
private:
    std::string _usrname;
    std::string _salt;
};

