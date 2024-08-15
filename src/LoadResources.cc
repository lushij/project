#include "LoadResources.h"
shared_ptr<LoadResources> LoadResources::_ptr;//单例模式
once_flag LoadResources::_initflag;

LoadResources::LoadResources()
    :_configs(Configuration::getInstance()->getConfigMap())
{
    /*
    en_dict_path ../data/dict_and_dict_idx/dict_en.dat
    en_idx_path ../data/dict_and_dict_idx/dict_en_idx.dat
    zn_dict_path ../data/dict_and_dict_idx/dict_zn.dat
    zn_idx_path ../data/dict_and_dict_idx/dict_zn_idx.dat
    */
    load_en_dict(_configs["en_dict_path"]);
    load_en_dict_idx(_configs["en_idx_path"]);
    load_zn_dict(_configs["zn_dict_path"]);
    load_zn_dict_idx(_configs["zn_idx_path"]);
}

shared_ptr<LoadResources> &LoadResources::getInstance()
{
    call_once(_initflag, []()
              { _ptr.reset(new LoadResources()); });
    return _ptr;
}

void LoadResources::load_en_dict(string &path)
{
    // 加载英文字典
    ifstream ifs(path);
    if (!ifs)
    {
        cerr << "Failed to open directory: " << path << "\n";
        ::exit(-1); // 如果无法打开目录，直接返回
    }
    string line;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        string word;
        int frequency;
        while (iss >> word >> frequency)
        {
            _en_dict.push_back({word, frequency});
        }
    }
    ifs.close();
}
void LoadResources::load_en_dict_idx(string &path)
{
    // 加载英文索引
    // unordered_map<string,set<int>>
    ifstream ifs(path);
    if (!ifs)
    {
        cerr << "Failed to open directory: " << path << "\n";
        ::exit(-1); // 如果无法打开文件，直接返回
    }
    string line;
    while (getline(ifs, line))
    {
        string word;
        istringstream iss(line);
        iss >> word;
        int num;
        while (iss >> num)
        {
            _en_dict_idx[word].insert(num);
        }
    }
    ifs.close();
}
void LoadResources::load_zn_dict(string &path)
{
    // 加载中文字典
    ifstream ifs(path);
    if (!ifs)
    {
        cerr << "Failed to open directory: " << path << "\n";
        ::exit(-1); // 如果无法打开文件，直接返回
    }
    string line;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        string word;
        int frequency;
        while (iss >> word >> frequency)
        {
            _zn_dict.push_back({word, frequency});
        }
    }
    ifs.close();
}
void LoadResources::load_zn_dict_idx(string &path)
{

    // 加载中文索引
    // unordered_map<string,set<int>>
    ifstream ifs(path);
    if (!ifs)
    {
        cerr << "Failed to open directory: " << path << "\n";
        ::exit(-1); // 如果无法打开文件，直接返回
    }
    string line;
    while (getline(ifs, line))
    {
        string word;
        istringstream iss(line);
        iss >> word;
        int num;
        while (iss >> num)
        {
            _zn_dict_idx[word].insert(num);
        }
    }
    ifs.close();
}

vector<pair<string, int>> &LoadResources::get_en_dict()
{
    return _en_dict;
}
unordered_map<string, set<int>> &LoadResources::get_en_dict_idx()
{
    return _en_dict_idx;
}

vector<pair<string, int>> &LoadResources::get_zn_dict()
{
    return _zn_dict;
}
unordered_map<string, set<int>> &LoadResources::get_zn_dict_idx()
{
    return _zn_dict_idx;
}