#pragma once
#include "myhead.h"
#include "Configuration.h"
#include "CppJieba.h"
#include "LoadResources.h"
#include "nlohmann/json.hpp"
#include <functional> // 如果使用 std::function
using std::function;
using json = nlohmann::json;
//关键字推荐类

//数据结构
struct Keyans
{
    string word;//单词或汉字
    int editLen;//最小编辑距离
    int freq;//频率
};

class KeyansComparator
{
public:
    bool operator()(const Keyans &lhs, const Keyans &rhs) const
    {
        if (lhs.editLen == rhs.editLen)
        {
            // 最小编辑距离相等看频率
            return lhs.freq > rhs.freq; // Min-heap requires less than to prioritize smaller frequencies
        }
        else
        {
            return lhs.editLen > rhs.editLen; // Min-heap requires less than to prioritize smaller editLen
        }
    }
};

class Keyword
{
public:
    Keyword(string word);
    ~Keyword(){}
    //求取一个字符占据的字节数
    size_t nBytesCode(const char ch);
    //求取一个字符串的字符长度
    size_t length(const string &str);
    int triple_min(const int &a, const int &b, const int &c);
    //中英文通用的最小编辑距离
    int editDistance(const string & lhs, const string &rhs);
    vector <string> split_utf8_everyone(string &input);
    void do_search_key();
    //计算最短编辑距离
    void calculate_edit();
    json get_send_msg();
private:
    string _queryWord;//待查询词
    vector<pair<string,int>>_similarity_word;//存放待查询词拆分对应的索引--->对应的词
    //默认是大根堆
    priority_queue<Keyans,vector<Keyans>,KeyansComparator>_rest_que;//优先级队列，回复客户端前几个数据
};

