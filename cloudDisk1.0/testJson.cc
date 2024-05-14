#include <iostream>
#include <string>
#include <stdio.h>
#include <nlohmann/json.hpp>
using Json = nlohmann::json;
using namespace std;

void test()
{
     auto text = R"({"filepath":"tmp/论文架构.png","filsSha1":"dd13a3148ae2e07de07b878a09c7c133fa5c2f2b","osspath":"disk/dd13a3148ae2e07de07b878a09c7c133fa5c2f2b"}
    )";
     Json jInfo = Json::parse(text);
     cout<<jInfo["filepath"]<<endl;
}
int main()
{
    test();
    return 0;
}

