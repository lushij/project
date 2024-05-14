#include <iostream>
#include <string>
#include <stdio.h>
#include "rabbitmq.hpp"
#include <SimpleAmqpClient/SimpleAmqpClient.h>
using namespace std;
int main(){
    // 指定mq的一些信息
    RabbitMqInfo MqInfo;
    // 创建一条和mq的连接
    AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create();
    // pause();
    // 创建消息
    auto  mesg = R"({"filepath":"tmp/论文架构.png","osspath":"disk/dd13a3148ae2e07de07b878a09c7c133fa5c2f2b"}
)";
    AmqpClient::BasicMessage::ptr_t message = AmqpClient::BasicMessage::Create(mesg);
    // 发布消息
    channel->BasicPublish(MqInfo.TransExchangeName,MqInfo.TransRoutingKey,message);
}

