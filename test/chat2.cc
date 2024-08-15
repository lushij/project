#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>


int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[4096];

    // 创建套接字
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    // 配置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 绑定套接字
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind failed" << std::endl;
        close(server_fd);
        return 1;
    }

    // 开始监听
    if (listen(server_fd, 10) == -1) {
        std::cerr << "Listen failed" << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "Waiting for connections..." << std::endl;

    // 接受客户端连接
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_fd == -1) {
        std::cerr << "Accept failed" << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "Client connected!" << std::endl;

    // 接收数据
    ssize_t received_bytes = recv(client_fd, buffer, sizeof(buffer), 0);
    if (received_bytes > 0) {
        std::string received_data(buffer, received_bytes);
        std::cout << "Received data: " << received_data << std::endl;
    } else if (received_bytes == 0) {
        std::cout << "Connection closed by client" << std::endl;
    } else {
        std::cerr << "Receive failed" << std::endl;
    }

    // 清理
    close(client_fd);
    close(server_fd);

    return 0;
}

