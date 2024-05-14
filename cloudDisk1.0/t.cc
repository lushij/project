#include "sendPhoneMsg.hpp"
#include<string>
int main()
{
    std::string accessKeyId = getenv("ALIBABA_CLOUD_ACCESS_KEY_ID");
    std::string accessKeySecret = getenv("ALIBABA_CLOUD_ACCESS_KEY_SECRET");
    sendPhoneMsg msgSender(accessKeyId, accessKeySecret);
    
    string code = phoneCode().get_code();
    fprintf(stderr,"code = %s\n",code.c_str());
    if (!msgSender.send("18552335201", code)) {
        return 1;
    }
    return 0;
}
