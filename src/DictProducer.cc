#include "DictProducer.h"

DictProducer::DictProducer()
    : _configs(Configuration::getInstance()->getConfigMap()), _cuttor(Cppjieba::getSplitTool()), _dict(), _dict_idx(), _dict_frequent()
{
    offlineBuildEnDict(); // 建立英文
    createEnIdx();
}

DictProducer::DictProducer(SplitTool *cuttor)
    : _configs(Configuration::getInstance()->getConfigMap()), _cuttor(cuttor), _dict(), _dict_idx(), _dict_frequent()
{
    offlineBuildZnDict(); // 建立中文
    createZnIdx();
}
size_t DictProducer::getByteNum_UTF_8(const char byte)
{
    int byteNum = 0;
    for (size_t i = 0; i < 6; ++i)
    {
        if (byte & (1 << (7 - i)))
            ++byteNum;
        else
            break;
    }
    return byteNum == 0 ? 1 : byteNum;
}
void DictProducer::offlineBuildEnDict() // 离线阶段 建立英文词典
{
    cout << "deal offlineBuildEnDict" << "\n";
    // 先获取停用词
    unordered_set<string> stopWordList = Configuration::getInstance()->getStopWordList();
    // 打开原始英文数据文件路径
    ifstream ifs(_configs["base_en"]);
    printf("open %s ...!\n", _configs["base_en"].c_str());
    if (!ifs)
    {
        cerr << _configs["base_en"] << "open err !" << "\n";
        mylog::LogError("%s open error !", _configs["base_en"].c_str());
        return;
    }
    ifs.seekg(std::ios_base::beg); // 重置文件描述符到起始位置
    string line;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        string words;

        while (iss >> words)
        {
            string insert_word;
            for (auto &ch : words)
            {
                if (isalpha(ch)) // 是字母再做处理
                {
                    insert_word += tolower(ch); // 全部转换小写字母
                }
            }
            // 查找是否是停用词
            if (!insert_word.empty())
            {
                auto stop_it = stopWordList.find(insert_word);
                if (stop_it != stopWordList.end())
                {
                    // 表示查到了，继续下一步，不做插入操作
                    continue;
                }
                else
                {
                    // 不是停用词，做数据插入和更新词频操作
                    ++_dict_frequent[insert_word];
                }
            }
        }
    }
    ifs.close();
    // 数据处理后，按照词频降序排列，存入磁盘文件中
    writeDict(_configs["en_dict_path"]);
}
void DictProducer::writeDict(string &path)
{
    // unordered_map不能排序
    for (auto &val : _dict_frequent)
    {
        _dict.push_back(val);
    }
    std::sort(_dict.begin(), _dict.end(),
              [](const std::pair<std::string, int> &a, const std::pair<std::string, int> &b)
              {
                  return a.second > b.second;
              });
    // 写入文件
    /*
        en_dict_path ../data/dict/dict_en.dat  英文词典
        en_idx_path ../data/dict/dict_en_idx.dat 英文词典索引
        zn_dict_path ../data/dict/dict_zn.dat
        zn_idx_path ../data/dict/dict_zn_idx.dat
    */
    ofstream ofs(path);
    if (!ofs)
    {
        cerr << path << "open err !" << "\n";
        mylog::LogError("%s open error !", path.c_str());
        return;
    }
    for (auto &val : _dict)
    {
        ofs << val.first << " " << val.second << "\n";
    }
    ofs.close();
}
void DictProducer::offlineBuildZnDict() // 建立中文词典
{
    cout << "deal offlineBuildZnDict" << "\n";
    // 先获取停用词
    unordered_set<string> stopWordList = Configuration::getInstance()->getStopWordList();
    // 打开原始英文数据文件路径
    _fileList = Configuration::getInstance() ->getFileList(_configs["base_zn"]);
    for (auto &path : _fileList)
    {
        cout<<"now deal path = "<<path<<"\n";
        ifstream ifs(path, std::ios::ate); // 打开文件，并把ptr指向文件末尾
        //printf("open %s ...!\n",path.c_str());
        if (!ifs)
        {
            cerr << path << "open err !" << "\n";
            mylog::LogError("%s open error !", path.c_str());
            return;
        }
        // 整篇文章一次性加载去分词
        size_t length = ifs.tellg();
        ifs.seekg(std::ios_base::beg); // 重置文件描述符到起始位置
        char *buf = new char[length + 1];
        ifs.read(buf, length + 1);
        string line(buf);
        vector<string> words = _cuttor->cut(line);
        for (auto &val : words)
        {
            // 不是停用词 && utf-8字节数为3
            auto it = stopWordList.find(val);
            if (it == stopWordList.end() && getByteNum_UTF_8(val[0]) == 3)
            {
                ++_dict_frequent[val];
            }
        }
        ifs.close();
    }
    writeDict(_configs["zn_dict_path"]);
}

void DictProducer::createZnIdx() // 处理中文词典索引
{
    int i = 0; // 记录下标
    for (auto &val : _dict)
    {
        /*
        vector<pair<string,int>>_dict;//词 词频 -> 词典
        unordered_map<string,set<int>>_dict_idx;//字/字母 含有字/字母的词的下标-->词典索引  在线阶段
        */
        // 中文处理方法
        // 武汉 666 排第一个 下标为0
        // 处理索引结果
        // 武 0
        // 汉 0
        string word = val.first;
        size_t charNums = word.size() / getByteNum_UTF_8(word[0]);
        for (size_t idx = 0, n = 0; n != charNums; ++idx, ++n) // 得到字符数
        {
            // 按照字符个数切割
            size_t charLen = getByteNum_UTF_8(word[idx]);
            string subWord = word.substr(idx, charLen); // 按照编码格式进行拆解
            _dict_idx[subWord].insert(i);//插入下标
            idx += (charLen - 1);
        }
        ++i;
    }
    writeIdx(_configs["zn_idx_path"]);
}

void DictProducer::createEnIdx() // 生成英文词典索引
{
    /*
    vector<pair<string,int>>_dict;//词 词频 -> 词典
    unordered_map<string,set<int>>_dict_idx;//字/字母 含有字/字母的词的下标-->词典索引  在线阶段
    */
    for (size_t i = 0; i < _dict.size(); ++i)
    {
        /*
            hello 666  排第一个，下标为0
            索引如 h 0
                  e 0
                  l 0
                  l 0
                  o 0
        */
        for (auto &ch : _dict[i].first)
        {
            _dict_idx[string(1, ch)].insert(i); // 将字符转化为单字符 字符串
        }
    }
    writeIdx(_configs["en_idx_path"]);
}

void DictProducer::writeIdx(string &path) // 存储索引文件
{
    ofstream ofs(path);
    if (!ofs)
    {
        cerr << path << "open err !" << "\n";
        mylog::LogError("%s open error !", path.c_str());
        return;
    }
    for (auto &val : _dict_idx)
    {
        ofs << val.first;
        for (auto &v : val.second)
        {
            ofs << " " << v;
        }
        ofs << "\n";
    }
    ofs.close();
}

DictProducer::~DictProducer()
{
}
