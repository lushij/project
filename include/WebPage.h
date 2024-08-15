#pragma once
#include "myhead.h"
#include "Configuration.h"
#include "Simhasher.hpp"
using namespace simhash;
class WebPage
{
public:
    WebPage(int id,const string &url,const string &title,const string &content);
    ~WebPage() {}
    bool operator < (const WebPage&rhs)
    {
        return _docId < rhs._docId;
    }
public:
    int _docId;//文章id
    string _docTitle;//文章标题
    string _docUrl;//文章url
    string _docContent;//文章内容
    // string _docSummary;//文章摘要 ---> 自动生成 ---> 放到queryWeb类了
    map<string, int> _wordsMap;//文章所有词语和词频
    double _sortWeight;//对网页进行排序的依据,权重越大越靠前
};

