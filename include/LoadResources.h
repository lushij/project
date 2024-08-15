#pragma once
#include "myhead.h"
#include "Configuration.h"
#include "CppJieba.h"
#include <memory>
using std::shared_ptr;
using std::call_once;
using std::once_flag;
class LoadResources
{
public:
    static shared_ptr<LoadResources>&getInstance();
    vector<pair<string,int>>& get_en_dict();
    unordered_map<string,set<int>>& get_en_dict_idx();
    vector<pair<string,int>>& get_zn_dict();
    unordered_map<string,set<int>>& get_zn_dict_idx();
    ~LoadResources(){}
private:
    LoadResources();
    void load_en_dict(string &path);
    void load_en_dict_idx(string &path);
    void load_zn_dict(string &path);
    void load_zn_dict_idx(string &path);
private:
    static shared_ptr<LoadResources> _ptr;//单例模式
    static once_flag _initflag;
    unordered_map<string,string> _configs;
    vector<pair<string,int>>_en_dict;//词 词频 -> 词典 
    unordered_map<string,set<int>>_en_dict_idx;//字/字母 含有字/字母的词的下标-->词典索引  在线阶段
    vector<pair<string,int>>_zn_dict;//词 词频 -> 词典 
    unordered_map<string,set<int>>_zn_dict_idx;//字/字母 含有字/字母的词的下标-->词典索引  在线阶段
};

