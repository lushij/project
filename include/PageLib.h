#pragma once
#include "myhead.h"
#include "Configuration.h"
#include "CppJieba.h"
#include "tinyxml2.h"
#include "Simhasher.hpp"
using namespace simhash;
using namespace tinyxml2;
/*
本类作用是网页查询的离线阶段所需处理的任务
清洗xml数据 ---> tinyxml2 ---> rss类型xml文件
形成网页库  特定格式的xml文本
网页偏移库  docid 起点 大小

*/
struct HashKey
{
    bool operator()(const uint64_t &lhs, const uint64_t &rhs) const
    {
        return simhash::Simhasher::isEqual(lhs, rhs);//去重算法得到的海明码，海明码一致表示一样
    }
};

class PageLib
{
public:
    PageLib();
    ~PageLib() {}
    void create_page_offset();//创建生成网页库和网页偏移库
    void write_page_offset();//网页库和网页偏移库--->存入磁盘
private:
    unordered_map<string, string> _configs;
    vector<string>_files; 
    vector<string> _pages;
    Simhasher _simhasher;
    unordered_set<uint64_t, std::hash<uint64_t>, HashKey> _simhash_set;
};
