#pragma once
#include "myhead.h"
#include "Mylogger.h"
#include <mutex>
using std::unique_ptr;
using std::once_flag;
using std::call_once;
class Configuration
{
    Configuration(const Configuration &) = delete;
    Configuration(Configuration &&) = delete;
    Configuration &operator=(const Configuration &) = delete;
    Configuration &operator=(Configuration &&) = delete;
public:
    static Configuration*getInstance();//单例模式要静态
    unordered_map<string,string> & getConfigMap();
    unordered_set<string> &getStopWordList();//获取停用词集合内容
    const string &operator[](string &key);
    vector<string>getFileList(string &path);
    ~Configuration() {}
private:
    Configuration();
    void InitStopWrodList();
private:
    static unique_ptr<Configuration> _pConfig;
    static once_flag _initflag;
    static string _configPath;
    unordered_map<string,string> _configs;
    unordered_set<string> _stopWords;
};

