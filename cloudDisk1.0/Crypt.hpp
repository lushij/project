#pragma once
#include <string>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include"linuxheader.h"
#include <unistd.h>
#include <unistd.h>
#include <crypt.h>

class Crypt
{
public:
    Crypt(){}
    Crypt(const std::string &password_or_usrname)
        :_password_or_usrname(password_or_usrname)
    {
        //加密密码使用的盐值
        //std::string salt;
        srand(time(nullptr));
        for(int i=12;i>0;i--)
        {
            int x=rand()%10;
           _salt.append(std::to_string(x));
        }
        _cryptPassword = crypt(_password_or_usrname.c_str(),_salt.c_str());
    }
    std::string getsalt()const
    {
        return _salt;
    }
    char * getcrypt_password()const
    {
        return _cryptPassword;
    }
    std::string virpty(const std::string &password ,std::string& salt)
    {
        return std::string(crypt(password.c_str(),salt.c_str()));
    }
    ~Crypt() {}
private:
    std::string _salt;
    char* _cryptPassword;//密文
    std::string _password_or_usrname;
};

