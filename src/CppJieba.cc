#include "CppJieba.h" 
#include "Configuration.h"
SplitTool* Cppjieba::_splittool = nullptr;
Cppjieba::AutoRelease Cppjieba::_ar;
std::once_flag Cppjieba::_initflag;

Cppjieba::Cppjieba()
    :_configs(Configuration::getInstance()->getConfigMap())
    ,_jieba(_configs["dict_path"],_configs["hmm_path"],_configs["user_dict_path"],_configs["idf_path"], _configs["stop_words_path"])
{
    
}

SplitTool* Cppjieba::getSplitTool()
{
    //采用c++11的新特性
    //std::call_once 保证了 Cppjieba 实例的创建是线程安全的，
    //并且只会在第一次调用时执行。
    //这避免了多线程环境下可能出现的竞态条件问题
    std::call_once(_initflag,[](){
        _splittool = new Cppjieba();
    });
    return _splittool;
}

vector<string> Cppjieba::cut(const string &sentence) 
{
    vector<string> all_words;
    _jieba.CutAll(sentence,all_words);
    return all_words;
}
