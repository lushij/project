#include "queryWeb.h"
using namespace mylog;
queryWeb::queryWeb(const string &queryword)
    : _queryWord(queryword), _stopWords(Configuration::getInstance()->getStopWordList()), _query_word_DF(1), _configs(Configuration::getInstance()->getConfigMap())
{
    // cout << "_queryword = " << _queryWord << "\n";
    load_offset();
    load_InvertIndex(); // 倒排索引库
    // cout << "load_InvertIndex() load_InvertIndex()" << "\n";
    doQuery();
    // cout << "doQuery();" << "\n";
}
void queryWeb::load_offset()
{
    // 读取磁盘中的网页偏移库文件到内存里
    ifstream ifs(_configs["offset"]);

    int _docid, _begin, _size;
    while (ifs >> _docid >> _begin >> _size)
    {
        _offset.push_back(pair<int, int>{_begin, _size}); // 这样插入更高效
    }
    ifs.close();
}

void queryWeb::load_InvertIndex()
{
    ifstream ifs(Configuration::getInstance()->getConfigMap()["invertIndex"]);
    if (!ifs)
    {
        cerr << "load invertIndex error" << "\n";
        LogError("load error to invertIndex");
        return;
    }
    string line;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        string word;
        iss >> word;
        int docid;
        double w;
        while (iss >> docid >> w)
        {
            _InvertIndex[word].insert({docid, w});
        }
    }
    ifs.close();
}
void queryWeb::doQuery()
{
    SplitTool *tool = Cppjieba::getSplitTool();
    vector<string> split_words = tool->cut(_queryWord); // 把查询词切割，并把停用词去掉
    for (auto &v : split_words)
    {
        if (_stopWords.find(v) == _stopWords.end()) // 表示不是停用词
        {
            _use_word[v]++;
        }
    }
    if (_use_word.empty())
    {
        string ret = "404 NOT FOUND";
        _sendMsg = json(ret);
        return;
    }
    double totoal_w = 0;
    for (auto &v : _use_word)
    {
        double IDF = log2(1 / (_query_word_DF + 1));
        double w = static_cast<double>(v.second * IDF);
        totoal_w += (w * w);
        _TF_IDF_W.emplace(v.first, w);
    }
    double total_w2_base = 0; // 归一化后的权重平方和
    for (auto &v : _TF_IDF_W)
    {
        v.second /= sqrt(totoal_w); // 归一化后的权重w
        total_w2_base += v.second * v.second;
    }
    // 通过倒排索引查找网页库进行排序，排序采用余弦相似度
    sort_ret_web(total_w2_base);
}

void queryWeb::sort_ret_web(double &total_w2_base)
{
    unordered_map<string, set<pair<int, double>>> _exit_word_invert;
    unordered_map<int, set<double>> _id_w;
    for (auto &v : _use_word)
    {
        // 通过倒排索引查找  去掉停用词的待查询词 只要有一个不在索引表就表示不存在
        if (_InvertIndex.find(v.first) == _InvertIndex.end())
        {
            // 表示不存在
            string ret = "404 NOT FOUND";
            _sendMsg = json(ret);
            return;
        }
        else
        {
            // 插入 _InvertIndex[v.first] 中的内容到 _exit_word_invert[v.first]
            _exit_word_invert[v.first].insert(_InvertIndex[v.first].begin(), _InvertIndex[v.first].end());
        }
    }
    // 遍历这些词语存在的docid，计算每一个docid对应的文章的相似度
    for (auto &v : _exit_word_invert)
    {
        for (auto &val : v.second)
        {
            //     docid                权重
            _id_w[val.first].emplace(val.second);
        }
    }
    // 余弦相似度算法计算
    // unordered_map<int, set<double>> _id_w;//每一个id对一个的出现待查词的权重
    //   unordered_map<string,double>_TF_IDF_W;//归一化前以及归一化后的权重w
    //

    for (auto it = _id_w.begin(); it != _id_w.end(); ++it)
    {
        double total_cos = 0;
        int docid = (*it).first;
        // cout << "docid = " << docid << "\n";
        auto _iit = (*it).second.begin(); // set<double>
        double total_y2_w = 0;
        for (auto &base_w : _TF_IDF_W)
        {
            total_y2_w += (*_iit) * (*_iit);
            total_cos += base_w.second * (*_iit);
            ++_iit;
            if (_iit == it->second.end())
            {
                break; // 防止迭代器越界
            }
        }
        total_cos = total_cos / (sqrt(total_w2_base) * sqrt(total_y2_w));
        _sort_id_cos.push({docid, total_cos});
    }
}
string queryWeb::get_pagelist_id(int id)
{

    ifstream ifs(_configs["ripepage"]);
    if (!ifs)
    {
        cerr << "ripepage error" << "\n";
        LogError("ripepage error to _offset");
        return nullptr;
    }

    int docid = id;
    int begin = _offset[id-1].first;
    int _size = _offset[id-1].second;
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
    string ret = "标题：" + title  + " 摘要：" + createSummary(title+content) + " 链接：" + url ;
    ifs.close();
    // cout<<ret<<"\n";
    return ret;
}
string queryWeb::createSummary(string str)
{
    string _docSummary;
    // 摘要这里就简单一些
    unordered_map<string, string> _configs = Configuration::getInstance()->getConfigMap();
    Simhasher _simhasher(_configs["dict_path"], _configs["hmm_path"], _configs["idf_path"], _configs["stop_words_path"]);
    size_t topN = 5;
    vector<pair<string, double>> res;
    _simhasher.extract(str, res, topN);
    for (size_t idx = 0; idx < 5; ++idx)
    {
        _docSummary += res[idx].first; // 只保留前五个
        _docSummary += "  ";
    }
    return _docSummary;
}
json queryWeb::get_send_msg()
{
    vector<string> ret;
    ret.push_back("WEB");
    for (int idx = 0; idx < _sort_id_cos.size() && idx < 5; ++idx)
    {
        int doicd = _sort_id_cos.top().id;
        _sort_id_cos.pop();
        ret.push_back(get_pagelist_id(doicd));
    }
    _sendMsg = json(ret);
    // cout<<_sendMsg.dump()<<endl;
    return _sendMsg;
}