#include "Keyword.h"

Keyword::Keyword(string word)
    : _queryWord(word)
{
    // cout<<"search word = "<<_queryWord<<"\n";
    do_search_key();
}

// 求取一个字符占据的字节数
size_t Keyword::nBytesCode(const char ch)
{
    if (ch & (1 << 7))
    {
        int nBytes = 1;
        for (int idx = 0; idx != 6; ++idx)
        {
            if (ch & (1 << (6 - idx)))
            {
                ++nBytes;
            }
            else
                break;
        }
        return nBytes;
    }
    return 1;
}
// 求取一个字符串的字符长度
size_t Keyword::length(const string &str)
{
    size_t len = 0;
    for (size_t i = 0; i < str.size(); ++i)
    {
        int nBytes = nBytesCode(str[i]);
        i += (nBytes - 1);
        ++len;
    }
    return len;
}

int Keyword::triple_min(const int &a, const int &b, const int &c)
{
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

// 中英文通用的最小编辑距离
int Keyword::editDistance(const string &lhs, const string &rhs)
{
    // 计算最小编辑距离-包括处理中英文
    size_t lhs_len = length(lhs);
    size_t rhs_len = length(rhs);
    int editDist[lhs_len + 1][rhs_len + 1];
    for (size_t idx = 0; idx <= lhs_len; ++idx)
    {
        editDist[idx][0] = idx;
    }
    for (size_t idx = 0; idx <= rhs_len; ++idx)
    {
        editDist[0][idx] = idx;
    }
    std::string sublhs, subrhs;
    for (std::size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i,
                     ++lhs_idx)
    {
        size_t nBytes = nBytesCode(lhs[lhs_idx]);
        sublhs = lhs.substr(lhs_idx, nBytes);
        lhs_idx += (nBytes - 1);
        for (std::size_t dist_j = 1, rhs_idx = 0;
             dist_j <= rhs_len; ++dist_j, ++rhs_idx)
        {
            nBytes = nBytesCode(rhs[rhs_idx]);
            subrhs = rhs.substr(rhs_idx, nBytes);
            rhs_idx += (nBytes - 1);
            if (sublhs == subrhs)
            {
                editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j - 1];
            }
            else
            {
                editDist[dist_i][dist_j] = triple_min(editDist[dist_i][dist_j - 1] + 1,
                                                      editDist[dist_i - 1][dist_j] + 1,
                                                      editDist[dist_i - 1][dist_j - 1] + 1);
            }
        }
    }
    return editDist[lhs_len][rhs_len];
}

vector <string> Keyword::split_utf8_everyone(string &input)
{
    //中英文通用拆解
    vector <string> words;
    int len = input.length();
    int i = 0;

    while (i < len)
    {
        assert((input[i] & 0xF8) <= 0xF0);
        int next = 1;
        if((input[i] & 0x80) == 0x00)
        {}
        else if ((input[i] & 0xE0) == 0xC0)
        {
            next = 2;
        }
        else if ((input[i] & 0xF0) == 0xE0)
        {
            next = 3;
        }
        else if ((input[i] & 0xF8) == 0xF0)
        {
            next = 4;
        }
        words.push_back(input.substr(i, next));
        i += next;
    }
    return words;
}

void Keyword::do_search_key()
{
    //拆分待查询词 ---> 每一个词或字母为一个string --->  进行索引读取
    vector<string> _everyone = split_utf8_everyone(_queryWord);
    // for(auto &v :_everyone)
    // {
    //     cout<<"_everyone = "<<v<<"\n";
    // }
    set<int>en_set;//存储行号
    set<int>zn_set;
    shared_ptr<LoadResources> _pload = LoadResources::getInstance();
    for(auto & val : _everyone)
    {
        if((val[0] & 0x80) == 0x00)
        {
            //遍历英文
            // cout<<"遍历英文查询"<<"\n";
            if(_pload->get_en_dict_idx().find(val) != _pload->get_en_dict_idx().end())
            {
                //找出该字母对应的索引行号
                set<int>tmp = _pload->get_en_dict_idx().at(val);
                en_set.insert(tmp.begin(),tmp.end());
            }
        }
        else
        {
            //遍历中文
            // cout<<"遍历中文查询"<<"\n";
            if(_pload->get_zn_dict_idx().find(val) != _pload->get_zn_dict_idx().end())
            {
                //找出该字对应的索引行号
                set<int>tmp = _pload->get_zn_dict_idx().at(val);
                zn_set.insert(tmp.begin(),tmp.end());
            }
        }
    }
    //根据索引查找词典,而此时设计的索引及对应词典vector的下标
    for(auto & idx :zn_set)
    {
        _similarity_word.push_back(_pload->get_zn_dict()[idx]);
        // cout<<"相似词插入"<<_pload->get_zn_dict()[idx].first<<"\n";
    }
     for(auto & idx :en_set)
    {
        _similarity_word.push_back(_pload->get_en_dict()[idx]);
        // cout<<"相似词插入"<<_pload->get_en_dict()[idx].first<<"\n";
    }
    //然后开始计算最短编辑距离
    // cout<<"相似词集合个数"<<_similarity_word.size()<<"\n";
    calculate_edit();
}


//计算最短编辑距离
void Keyword::calculate_edit()
{
    /*
    struct Keyans
    {
        string word;//单词或汉字
        int editLen;//最小编辑距离
        int freq;//频率
    };
    */
    for(auto &val : _similarity_word)
    {
        _rest_que.push({val.first,editDistance(_queryWord,val.first),val.second});
    }

}
json Keyword::get_send_msg()
{
    //得到向客户端发送的json数据
    vector<string>ret;
    ret.push_back("KEY");
    int i=0;
    cout<<"que size = "<<_rest_que.size()<<"\n";
    while (!_rest_que.empty() && i < 3)
    {
        ret.push_back(_rest_que.top().word);
        // cout<<"_rest_que.top().word = "<<_rest_que.top().word<<"\n";
        _rest_que.pop();
        ++i;
    }
    // cout<<"json Keyword::get_send_msg() and ret size = "<<ret.size()<<"\n";
    return json(ret);
}