#include "Hash.hpp"

#include "linuxheader.h"

#include <openssl/sha.h>

#include <iostream>
using std::cout;
using std::endl;

string Hash::sha1() const
{
    int fd = open(_filename.c_str(), O_RDONLY);
    if(fd < 0) {
        perror("open");
        return string();
    }
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    char buff[1024] = {0};
    int ret = 0;
    while(1) {
        ret = read(fd, buff, 1024);
        if(ret == 0) {
            break;
        }

        SHA1_Update(&ctx, buff, ret);
        bzero(buff, 1024);
    }
    unsigned char md[20] = {0};
    SHA1_Final(md, &ctx);
    string result;
    char fragment[3] = {0};
    for(int i = 0; i < 20; ++i) {
        sprintf(fragment, "%02x", md[i]);
        result += fragment;
    }
    return result;
}


