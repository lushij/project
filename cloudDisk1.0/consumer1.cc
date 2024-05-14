#include "rabbitmq.hpp"
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <alibabacloud/oss/OssClient.h>
#include "oss.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <map>
using std::map;
using std::cout;
using std::string;
using std::endl;
using Json = nlohmann::json;

// 手写提取函数
std::map<std::string, std::string> kv_parse(std::string& jsonString) 
{
    std::map<std::string, std::string> keyValuePairs;
    std::string key;
    std::string value;
    // 找到 "filepath" 的位置
    auto it = jsonString.find("filepath");
    if (it != std::string::npos) 
    {
        // 找到 "osspath" 的位置
        auto endIt = jsonString.find("osspath");
        std::string tmp = jsonString.substr(endIt+10);
        if (endIt != std::string::npos) 
        {
            // 计算并获取子字符串
            value = jsonString.substr(it + 11, endIt - (it + 14)); // 14 = 11 (filepath") + 3 (",")
            cout<<value<<endl;
            // 插入键值对到 map 中
            keyValuePairs.insert({"filepath", value});
        }
        value = tmp.substr(0,45);
        keyValuePairs.insert({"osspath", value});
        cout<<value<<endl;
    }
    return keyValuePairs;
}



int main() {
    AlibabaCloud::OSS::InitializeSdk();

    // 指定mq的一些信息
    RabbitMqInfo MqInfo;

    // 创建一条和mq的连接
    AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create();

    while(1)
    {
        // 从mq中提取消息
        channel->BasicConsume(MqInfo.TransQueueName, MqInfo.TransQueueName);
        AmqpClient::Envelope::ptr_t envelope;
        bool isNotTimeout = channel->BasicConsumeMessage(envelope, 5000);
        if (!isNotTimeout) {
            std::cerr << "Timeout" << std::endl;
            continue;
        }
        std::string messageBody = envelope->Message()->Body();
        std::cout << "Message: " << messageBody << std::endl;
        map<std::string, std::string> Info = kv_parse(messageBody);
#if 1
        OSSInfo info;
        AlibabaCloud::OSS::ClientConfiguration conf;
        AlibabaCloud::OSS::OssClient client(info.Endpoint, info.AccessKeyId, info.AccessKeySecret, conf);
        auto outcome = client.PutObject(info.Bucket, Info["osspath"], Info["filepath"]); //把本地的filepath存入OSS的osspath
        if (!outcome.isSuccess()) {
            std::cerr << "PutObject fail, code = " << outcome.error().Code()
                << ", message = " << outcome.error().Message()
                << ", request id = " << outcome.error().RequestId() << std::endl;
        } else {
            std::cout << "PutObject success" << std::endl;
        }
#endif
    }
    AlibabaCloud::OSS::ShutdownSdk();
    return 0;
}

