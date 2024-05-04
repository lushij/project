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
struct Config{
    int storeType = OSS;
    int isAsyncTransferEnable = true;
};


