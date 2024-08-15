#include <iostream>
#include <string>
#include <regex>

int main() {
    std::string input = "标题：中国海油启动碳中和规划全面推动公司绿色低碳转型 摘要：海油  低碳  LNG  绿色  排放  链接：http://ccnews.people.com.cn/n1/2021/0118/c141677-32002853.html";

    // 正则表达式匹配“摘要”和“链接”并在其前后插入换行符
    input = std::regex_replace(input, std::regex("(摘要：)"), "\n$1");
    input = std::regex_replace(input, std::regex("(链接：)"), "\n$1");

    // 输出格式化后的字符串
    std::cout << input << std::endl;

    return 0;
}

