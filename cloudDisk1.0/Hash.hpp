#pragma once
#include <string>
#include <stdlib.h>
#include <time.h>
using std::string;

//对文件内容进行sha1加密算法
class Hash
{
public:
    Hash(){}
    Hash(const string & filename) 
    : _filename(filename)
    {}

    ~Hash() {}

    string sha1() const;
private:
    string _filename;
};

