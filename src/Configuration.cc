#include "Configuration.h"
unique_ptr<Configuration> Configuration::_pConfig = nullptr;
once_flag Configuration::_initflag;
string Configuration::_configPath = "../conf/server.conf";

Configuration::Configuration()
{
    mylog::LogInfo("读取配置文件ing");
    std::ifstream ifs(_configPath);
    std::string first, second;
    if (!ifs)
    {
        std::cerr << _configPath << " not exist" << std::endl;
        mylog::LogError("not exit!");
        ::exit(1);
    }
    while (ifs >> first >> second)
    {
        _configs.insert({first, second});
    }
    ifs.close();
    mylog::LogInfo("读取完成");
}

Configuration *Configuration::getInstance()
{
    call_once(_initflag, []()
              { _pConfig.reset(new Configuration()); });
    return _pConfig.get();
}

unordered_map<string, string> &Configuration::getConfigMap()
{
    // if(_configs.empty())
    // {
    //     _configs["dict_path"]="../../include/cppjieba/dict/jieba.dict.utf8";
    //     _configs["hmm_path"] = "../../include/cppjieba/dict/hmm_model.utf8";
    //     _configs["user_dict_path"] = "../../include/cppjieba/dict/user.dict.utf8";
    //     _configs["idf_path"] = "../../include/cppjieba/dict/idf.utf8";
    //     _configs["stop_words_path"] = "../../include/cppjieba/dict/stop_words.utf8";
    // }
    return _configs;
}

unordered_set<string> &Configuration::getStopWordList() // 获取停用词集合内容
{
    InitStopWrodList();
    return _stopWords;
}

void Configuration::InitStopWrodList()
{
    vector<string> _fileList = getFileList(_configs["stop"]);
    for (auto &path : _fileList)
    {
        ifstream ifs(path);
        if (!ifs)
        {
            cerr << "StopWordListInit open error !" << "\n";
            return;
        }
        ifs.seekg(std::ios_base::beg); // 将文件描述符重置开始位置
        string word;
        while (getline(ifs, word))
        {
            if (word.back() == '\r')
            {
                // 把换行符去掉
                word.pop_back();
            }
            _stopWords.insert(word); // 插入处理后的停用词
        }
        ifs.close();
    }
    cout<<"_stopWords size = "<<_stopWords.size()<<"\n";
}
const string &Configuration::operator[](string &key)
{
    return _configs.at(key);
}

vector<string> Configuration::getFileList(string &path)
{
    vector<string> _fileList;
    DIR *dir = opendir(path.c_str());
    if (!dir)
    {
        cerr << "Failed to open directory: " << path << "\n";
        ::exit(-1); // 如果无法打开目录，直接返回
    }
    struct dirent *_dirent;
    while ((_dirent = readdir(dir)) != NULL)
    {
        if (((string)_dirent->d_name) != "." && ((string)_dirent->d_name) != "..")
        {
            _fileList.push_back(path + "/" + (string)_dirent->d_name);
        }
    }
    // 关闭目录，避免资源泄漏
    closedir(dir);
    // for (auto &v : _fileList)
    // {
    //      //cout<<"DEBUG--253"<<"\n";
    //     cout << v << "\n";
    // }
    return _fileList;
}