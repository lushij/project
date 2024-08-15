#ifndef _CPPJIEBA_H_
#define _CPPJIEBA_H_
#include "cppjieba/Jieba.hpp"
#include "myhead.h"
#include <mutex>

class SplitTool
{
public:
    //分词函数，纯虚函数，提供接口
    virtual vector<string> cut(const string & sentence) = 0;
    //虚析构函数
    virtual ~SplitTool() {}
};

//此类是分词工具类,由于cppjieba库初始化占用时间，故采用单利模式
class Cppjieba
:public SplitTool
{
public:
    static SplitTool*getSplitTool();
    virtual vector<string> cut(const string &sentence) override;
private:
    Cppjieba();
    virtual ~Cppjieba() {}
    class AutoRelease
    {
    public:
        ~AutoRelease()
        {
            if(_splittool)
            {
                delete _splittool;
                _splittool = nullptr;
            }
        }
    };
private:
    unordered_map<string,string> &_configs;//配置文件
    cppjieba::Jieba _jieba;
    static SplitTool *_splittool;
    static AutoRelease _ar;//内部类+静态栈上成员函数==单例模式自动释放
    static std::once_flag _initflag;
};

#endif
