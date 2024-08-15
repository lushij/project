#include "PageLib.h"
#include "Mylogger.h"
using namespace mylog;
PageLib::PageLib()
    : _configs(Configuration::getInstance()->getConfigMap())
    , _simhasher(_configs["dict_path"], _configs["hmm_path"], _configs["idf_path"], _configs["stop_words_path"])
    , _files(Configuration::getInstance()->getFileList(_configs["xml_path"]))
{
}
void PageLib::create_page_offset() // 创建生成网页库和网页偏移库
{
    XMLDocument doc;
    size_t topN = 15;
    for (auto &path : _files)
    {
        // 用tinyxml2解析xml文件生成特定格式
        doc.LoadFile(path.c_str());
        if (doc.ErrorID())
        {
            cerr << "Load file fail" << "\n";
            LogError("Load file fail");
            return;
        }
        XMLElement *itemNode = doc.FirstChildElement("rss")->FirstChildElement("channel")->FirstChildElement("item");
        while (itemNode)
        {
            std::string title = itemNode->FirstChildElement("title")->GetText();
            std::string url = itemNode->FirstChildElement("link")->GetText();
            std::string content = itemNode->FirstChildElement("content") ? itemNode->FirstChildElement("content")->GetText()
                                  : itemNode->FirstChildElement("description") ? itemNode->FirstChildElement("description")->GetText()
                                                                               : "";
            //上面这是嵌套语句 
            std::regex reg("( |　|&nbsp;|[\r]|[\n]|<[^>]+>|showPlayer[(]+[^)]+[)];)");
            title = std::regex_replace(title, reg, "");
            content = std::regex_replace(content, reg, "");
            uint64_t contentHash;
            content.empty() ? _simhasher.make(title, topN, contentHash) : _simhasher.make(content, topN, contentHash);
            if (_simhash_set.insert(contentHash).second)//insert返回pair,其中first表示插入成功的迭代器，second表示是否插入成功--->bool
            {
                //能插进去表示不重复
                _pages.emplace_back(std::string("<doc>\n\t<docid>") + std::to_string(_pages.size() + 1) + "</docid>\n\t<url>" + url + "</url>\n\t<title>" + title + "</title>\n\t<content>" + content + "</content>\n</doc>\n");
            }
            itemNode = itemNode->NextSiblingElement("item");
        }
    }
}
void PageLib::write_page_offset() // 网页库和网页偏移库--->存入磁盘
{
    ofstream ofs_page(_configs["ripepage"]);//网页库
    ofstream ofs_offset(_configs["offset"]);//网页偏移库
    if(!ofs_page || !ofs_offset)
    {
        LogError("void PageLib::write_page_offset() failed");
        return;
    }
    std::fstream::pos_type p1,p2;//文件标志位
    //先插入网页库
    for(size_t idx = 0 ; idx != _pages.size(); ++ idx)
    {
        //每一篇的开始插入网页库位置
        p1 = ofs_page.tellp();
        ofs_page <<_pages[idx];
        //每一篇插入网页库的位置
        p2 = ofs_page.tellp();
        //计算偏移库数据
        //docid 起始位置 len
        ofs_offset << idx + 1 <<" "<<std::to_string(p1) <<" "<<std::to_string(p2 - p1)<<"\n";
    }
    ofs_offset.close();
    ofs_page.close();
}
