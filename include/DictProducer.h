#pragma once
#include "myhead.h"
#include "CppJieba.h"
#include "Configuration.h"
#include "Mylogger.h"
/*
本类是生成字典的类，中英文字典
*/
class DictProducer
{
public:
    DictProducer();
    DictProducer(SplitTool * cuttor);//建立中文
    void offlineBuildEnDict();//建立英文词典--->离线阶段
    void offlineBuildZnDict();//建立中文词典--->离线阶段
    void createZnIdx();//生成中文词典索引
    void createEnIdx();//生成英文词典索引
    void writeIdx(string &path);//存储索引文件
    void writeDict(string &path);//存储词典
    size_t getByteNum_UTF_8(const char byte);
    ~DictProducer();

private:
    unordered_map<string,int>_dict_frequent;//离线阶段 词频统计 --->不能排序
    vector<string> _fileList;
    vector<pair<string,int>>_dict;//词 词频 -> 词典 
    unordered_map<string,set<int>>_dict_idx;//字/字母 含有字/字母的词的下标-->词典索引  在线阶段
    SplitTool * _cuttor;
    unordered_map<string,string>&_configs;//配置文件
};

