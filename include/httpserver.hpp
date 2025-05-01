#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

class HttpServer
{
public:
    explicit HttpServer(uint16_t port = 1234);
    ~HttpServer();
    void start();

private:
    static const size_t k_max_msg = 4096;
    int m_listen_fd;
    uint16_t m_port;

    void setup_server();             // 初始化服务器socket方法
    void listen_for_connections();    // 监听并处理连接
    void handle_client(int connfd);  // 处理单个客户端连接
    int process_request(int connfd); // 处理单个请求

    /*辅助函数*/
    static int read_full(int fd, char *buf, size_t n);
    static int write_all(int fd, char *buf, size_t n);

    /*错误处理*/
    static void die(const char *msg);
    static void log_error(const char *msg);
};