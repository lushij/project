#include <alibabacloud/oss/OssClient.h>
#include <string>
#include "oss.hpp"
using namespace AlibabaCloud::OSS;
int main(){
    //初始化网络连接
    InitializeSdk();
    OSSInfo info;
    //配置客户端属性（默认属性）
    ClientConfiguration conf;
    //创建一个客户端
    OssClient client(info.Endpoint,info.AccessKeyId,info.AccessKeySecret,conf);
    std::string filepath = "oss.hpp";
    auto outcome = client.PutObject(info.Bucket,"tmp/"+ filepath, filepath);//把本地的Token.h存入OSS的tmp/Token.h
    if(!outcome.isSuccess()){
        fprintf(stderr,"PutObject fail, code = %s, message = %s, request id = %s\n", outcome.error().Code().c_str()
                ,outcome.error().Message().c_str()
                ,outcome.error().RequestId().c_str());
    }
    time_t t = std::time(nullptr) + 1200;
    auto outcome1 = client.GeneratePresignedUrl(info.Bucket,"tmp/Token.h",t,Http::Get);
    if(!outcome.isSuccess()){
        fprintf(stderr,"PutObject fail, code = %s, message = %s, request id = %s\n", outcome.error().Code().c_str()
                ,outcome.error().Message().c_str()
                ,outcome.error().RequestId().c_str());
    }
    else{
        fprintf(stderr,"url = %s\n", outcome1.result().c_str());
    }
    //释放连接
    ShutdownSdk();
}
