#pragma once
#include <alibabacloud/oss/OssClient.h>
struct OSSInfo{
    std::string Bucket = "lushijibucket";
    std::string Endpoint = "oss-cn-beijing.aliyuncs.com";
    std::string AccessKeyId = "LTAI5tNhMBCmf5w3hPYfB176";
    std::string AccessKeySecret = "9BShG8dbdPomoLkDzBGbTqrknrJjd1"; 
};
enum {
    FS,
    OSS
};
//是否开启异步转移
struct Config{
    int storeType = OSS;
    int isAsyncTransferEnable = true;
};


