#include "../cppjieba/Jieba.hpp"
#include <string>
#include <vector>
using std::cout;
using std::vector;
using std::string;
//注意路径
const char* const DICT_PATH = "../cppjieba/dict/jieba.dict.utf8";
const char* const HMM_PATH = "../cppjieba/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../cppjieba/dict/user.dict.utf8";
const char* const IDF_PATH = "../cppjieba/dict/idf.utf8";
const char* const STOP_WORD_PATH = "../cppjieba/dict/stop_words.utf8";

int main()
{

     cppjieba::Jieba jieba(DICT_PATH,
        HMM_PATH,
        USER_DICT_PATH,
        IDF_PATH,
        STOP_WORD_PATH);//创建对象
    string s = "北京欢迎你";
    vector<string>ret;
    jieba.Cut(s,ret,true);
    for(auto &word : ret)
    {
        cout<<word<<"\n";
    }
    ret.clear();
    s="武汉欢迎你！";
    jieba.CutForSearch(s,ret);
    for(auto &word : ret)
    {
        cout<<word<<"\n";
    }

    return 0;
}

