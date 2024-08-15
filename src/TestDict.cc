#include "Configuration.h"
#include "CppJieba.h"
#include "DictProducer.h"
int main()
{
    #if 0
    string s = "武汉欢迎你";
    vector<string>ret;
    SplitTool * cppjieba = Cppjieba::getSplitTool();
    ret = cppjieba->cut(s);
    for(auto & word :ret)
    {
        cout<<word<<"\n";
    }
    #endif
    DictProducer EnDict;
    DictProducer ZnDict(Cppjieba::getSplitTool());
    return 0;
}

