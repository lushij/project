#pragma once
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <alibabacloud/core/AlibabaCloud.h>
#include <alibabacloud/core/CommonRequest.h>
#include <alibabacloud/core/CommonClient.h>
#include <alibabacloud/core/CommonResponse.h>

static std::string KeyId = getenv("ALIBABA_CLOUD_ACCESS_KEY_ID");
static std::string KeySecret = getenv("ALIBABA_CLOUD_ACCESS_KEY_SECRET");
using namespace std;
using namespace AlibabaCloud;

class sendPhoneMsg {
public:
    sendPhoneMsg(const string& accessKeyId=KeyId, const string& accessKeySecret=KeySecret)
        : m_accessKeyId(accessKeyId)
          , m_accessKeySecret(accessKeySecret) 
    {}

    bool send(const string& phoneNumber, const string& code) {
        InitializeSdk();
        ClientConfiguration configuration("cn-shanghai");
        configuration.setConnectTimeout(1500);
        configuration.setReadTimeout(4000);
        Credentials credential(m_accessKeyId, m_accessKeySecret);
        CommonClient client(credential, configuration);
        CommonRequest request(CommonRequest::RequestPattern::RpcPattern);
        request.setHttpMethod(HttpRequest::Method::Post);
        request.setDomain("dysmsapi.aliyuncs.com");
        request.setVersion("2017-05-25");
        request.setQueryParameter("Action", "SendSms");
        request.setQueryParameter("SignName", "阿里云短信测试");
        request.setQueryParameter("TemplateCode", "SMS_154950909");
        request.setQueryParameter("PhoneNumbers", phoneNumber);
        request.setQueryParameter("TemplateParam", "{\"code\":\"" + code + "\"}");

        auto response = client.commonResponse(request);
        if (response.isSuccess()) {
            cout << "Request success." << endl;
            cout << "Result: " << response.result().payload() << endl;
            ShutdownSdk();
            return true;
        } else {
            cerr << "Error: " << response.error().errorMessage() << endl;
            cerr << "Request id: " << response.error().requestId() << endl;
            ShutdownSdk();
            return false;
        }
    }

private:
    string m_accessKeyId;
    string m_accessKeySecret;
};

class phoneCode
{
public:
    string get_code()
    {
        string code;
        srand(time(NULL));
        for(int i=0;i<6;i++)
        {
            int x=rand()%10;
            code+=to_string(x);
        }
        return code;
    }

};
