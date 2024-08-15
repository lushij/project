#include "nlohmann/json.hpp"
#include <iostream>
#include <string>
using json = nlohmann::json;
int main()
{
    // parse explicitly
    auto j3 = json::parse(R"({"happy": true, "pi": 3.141})");
    std::string s = j3.dump();
    std::cout<<s<<"\n";
    return 0;
}

