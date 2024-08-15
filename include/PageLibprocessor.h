#pragma once
//创建倒排索引库
#include"myhead.h"
#include"Configuration.h"
#include"CppJieba.h"
#include"WebPage.h"
class PageLibprocessor
{
public:
    PageLibprocessor();
    ~PageLibprocessor() {}
    void buildInvertIndex();
    void writeInvertIndex();
private:
    void load_offset();
    void load_pagelist();
private:
    //加载网页偏移库
    unordered_map<int,pair<int,int>>_offset;
    vector<WebPage>_pageList;//从网页偏移库查找网页库加载到这里
    unordered_map<string,set<pair<int,double>>> _InvertIndex;//倒排索引库格式
    SplitTool *_cut;//切割分词工具
    unordered_map<string,string>_configs;//配置文件获取
};

