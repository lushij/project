#pragma once
#include "myhead.h"
#include "CppJieba.h"
#include "Configuration.h"
#include "PageLibprocessor.h"
#include "WebPage.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
/*
在线阶段
1-->加载文件 网页库，网页偏移库，倒排索引库，停用词表
*/
struct id_cos
{
    int id;
    double cos;
};
struct sort_cos
{
    bool operator()(const id_cos &lhs, const id_cos &rhs)
    {
        return lhs.cos > rhs.cos;
    }
};
class queryWeb
{
public:
    queryWeb(const string &queryword);
    ~queryWeb() {}
    void doQuery();
    json get_send_msg();

private:
    void load_offset(); 
    void load_InvertIndex();
    void sort_ret_web(double &totoal_w);
    string get_pagelist_id(int id);
    string createSummary(string str);
private:
    unordered_map<string,string>_configs;//配置文件获取
    string _queryWord;
    int _query_word_DF;                                            // TF-IDF 算法中的DF
    vector<pair<int, int>> _offset;                    // 加载网页偏移库-->省内存
    // vector<WebPage> _pageList;                                     // 从网页偏移库查找网页库加载到这里
    unordered_map<string, set<pair<int, double>>> _InvertIndex;    // 倒排索引库
    unordered_set<string> _stopWords;                              // 停用词表
    unordered_map<string, double> _TF_IDF_W;                       // 归一化前以及归一化后的权重w
    unordered_map<string, int> _use_word;                          // 去掉停用词后的词语以及词频
    priority_queue<id_cos, vector<id_cos>, sort_cos> _sort_id_cos; // 余弦算法得到的id和余弦值
    json _sendMsg;
};
