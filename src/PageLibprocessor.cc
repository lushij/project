#include "PageLibprocessor.h"
#include "Mylogger.h"
using namespace mylog;

PageLibprocessor::PageLibprocessor()
    : _cut(Cppjieba::getSplitTool()), _configs(Configuration::getInstance()->getConfigMap())
{
    load_offset();
    load_pagelist();
}

void PageLibprocessor::load_offset()
{
    // 读取磁盘中的网页偏移库文件到内存里
    ifstream ifs(_configs["offset"]);

    int _docid, _begin, _size;
    while (ifs >> _docid >> _begin >> _size)
    {
        // unordered_map<int,pair<int,int>>_offset;
        _offset.emplace(_docid, pair<int, int>{_begin, _size}); // 这样插入更高效
    }
    ifs.close();
}
void PageLibprocessor::load_pagelist()
{
    //网页库
    //                      docid   begin size
    // 然后根据unordered_map<int,pair<int,int>>_offset;在网页库本地磁盘文件进行查找特定docid，提高效率
    ifstream ifs(_configs["ripepage"]);
    if (!ifs)
    {
        cerr << "ripepage error" << "\n";
        LogError("ripepage error to _offset");
        return;
    }
    for (auto &val : _offset)
    {
        int docid = val.first;
        int begin = val.second.first;
        int _size = val.second.second;
        char buf[_size + 1];
        ::bzero(buf, sizeof(buf));
        ifs.seekg(begin);
        ifs.read(buf, _size);
        string doc(buf);
        // 此时doc文件格式如下，摘取对应插入_pagelist中
        /*
        <doc>
            <docid>8</docid>
            <url>http://www.stats.gov.cn/tjsj/zxfb/202105/t20210517_1817511.html</url>
            <title>2021年1—4月份全国固定资产投资（不含农户）增长19.9%比2019年1—4月份增长8.0%</title>
            <content>1—4月份，全国固定资产投资（不含农户）增长18.7%。</content>
        </doc>
        */
        size_t url_begin_pos, title_begin_pos, content_begin_pos;
        size_t url_end_pos, title_end_pos, content_end_pos;
        url_begin_pos = doc.find("<url>") + 5; // find返回的是 <url>中<的位置，再加<url>本身的长度表示有效数据的起始位置
        title_begin_pos = doc.find("<title>") + 7;
        content_begin_pos = doc.find("<content>") + 9;
        url_end_pos = doc.find("</url>");
        title_end_pos = doc.find("</title>");
        content_end_pos = doc.find("</content>");
        string url = doc.substr(url_begin_pos, url_end_pos - url_begin_pos);
        string title = doc.substr(title_begin_pos, title_end_pos - title_begin_pos);
        string content = doc.substr(content_begin_pos, content_end_pos - content_begin_pos);
        _pageList.emplace_back(docid, url, title, content);
    }
    ifs.close();
}
void PageLibprocessor::buildInvertIndex()
{
    // 倒排索引所用的权重计算方法
    /*
        TF : Term Frequency, 某个词在文章中出现的次数；
        DF: Document Frequency, 某个词在所有文章中出现的次数，即包含该词语
        的文档数量；
        IDF: Inverse Document Frequency, 逆文档频率，表示该词对于该篇文章的重
        要性的一个系数，其计算公式为：
        IDF = log2(N/(DF+1))，其中 N 表示文档的总数或网页库的文档数
        最后，词语的权重 w 则为：
        w = TF * IDF
    */

    // 生成倒排索引库 格式 词          id   权重
    // unordered_map<string,set<pair<int,double>>> InvertIndex;
    // 遍历pagelist中的数据
    unordered_map<string, int> DF; // 用来跟踪每个单词在多少个文档中出现（文档频率）
    for (auto &v : _pageList)
    {
        vector<string> _cutWord;
        _cutWord = _cut->cut(v._docTitle + v._docContent);
        for (auto &word : _cutWord)
        {
            // 检查是否在该文档中
            if (!(v._wordsMap[word]++)) // 用来跟踪该单词在该文档中的出现频率，即TF
            {
                // 表示不在该文章中，首次遇见
                ++DF[word];
            }
        }
    }
    double total = static_cast<double>(_pageList.size()); // 网页库的总个数
    unordered_map<int, double> _doc_total_w;              // 归一化所用的每一篇的所有w*w的和
    // unordered_map<int,set<pair<string,double>>>_doc_id_w;//归一化前的每个id包含的词语和权重
    for (auto &doc : _pageList)
    {
        int docid = doc._docId;
        // 计算每一篇的权重
        for (auto &wordmap : doc._wordsMap) // 一个id对应一篇文章--->所有词频
        {
            // 生成倒排索引库 格式 词          id   权重
            // unordered_map<string,set<pair<int,double>>> InvertIndex;倒排索引格式
            double IDF = log2(total / (DF[wordmap.first] + 1));
            double w = wordmap.second * IDF;
            _InvertIndex[wordmap.first].emplace(docid, w); // 因为set不可修改，所以先插入原始，
            // _doc_id_w.emplace(docid,pair<string,double>{wordmap.first,w});
            _doc_total_w[docid] += w * w; // 每一个docid对应的一篇中所有词语权重平方和
        }
    }
    // 进行归一化
    /*
        一篇文档包含多个词语 w1,w2,...,wn，还需要对这些词语的权重系数进行归一化处理，其计算公式如下：
        w' = w /sqrt(w1^2 + w2^2 +...+ wn^2)
        w'才是需要保存下来的
    */
    for (auto &v : _InvertIndex)
    {
        //            id  权重
        // set<pair<int,double>> ---> v
        set<pair<int, double>> newSet;
        for (auto p : v.second) // 不能& ，否则不能修改计算
        {
            // pair<int,double>--->p
            p.second /= (sqrt(_doc_total_w[p.first])); // 权重
            newSet.insert(p);                          // 新旧替换
        }
        v.second = std::move(newSet);
    }
}

void PageLibprocessor::writeInvertIndex()
{
    ofstream ofs(_configs["invertIndex"]);
    if (!ofs)
    {
        cerr << "invertIndex error" << "\n";
        LogError("invertIndex error to invertIndex");
        return;
    }
    for (auto &val : _InvertIndex)
    {
    
        ofs << val.first;
        for (auto &v : val.second)
        {
            ofs << " " << v.first << " " << v.second;
        }
        ofs << "\n";
        
    }
    ofs.close();
}

