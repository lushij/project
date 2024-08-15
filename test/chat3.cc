#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>


void handleClient(int client_fd);

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

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

    while (true) {
        // 接受客户端连接
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd == -1) {
            std::cerr << "Accept failed" << std::endl;
            close(server_fd);
            return 1;
        }

        std::cout << "Client connected!" << std::endl;

        // 创建子进程处理客户端连接
        pid_t pid = fork();
        if (pid == 0) {
            // 子进程处理客户端连接
            close(server_fd); // 子进程不需要监听套接字
            handleClient(client_fd);
            close(client_fd);
            exit(0); // 处理完毕后子进程退出
        } else if (pid > 0) {
            // 父进程关闭客户端套接字继续接受新连接
            close(client_fd);
        } else {
            std::cerr << "Fork failed" << std::endl;
            close(client_fd);
            close(server_fd);
            return 1;
        }
    }

    // 清理
    close(server_fd);

    return 0;
}

void handleClient(int client_fd)
{
    std::cout<<client_fd<<"\n";
    char buffer[4096];
    while (true) {
        ssize_t received_bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (received_bytes > 0) {
            buffer[received_bytes] = '\0'; // 确保缓冲区是以null结尾的字符串
            std::string received_data(buffer);
            std::cout << "Received data: " << received_data << std::endl;
            send(client_fd,buffer,strlen(buffer)+1,0);
        } else if (received_bytes == 0) {
            std::cout << "Connection closed by client" << std::endl;
            break;
        } else {
            std::cerr << "Receive failed: " << strerror(errno) << std::endl;
            break;
        }
    }

    // 关闭客户端连接
    close(client_fd);
}

