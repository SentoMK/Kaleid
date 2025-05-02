#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

class HttpClient
{
public:
    explicit HttpClient(const char *host, uint16_t port);
    ~HttpClient();

    void send_request(const std::string &msg);
    std::string receive_response();

    // 禁止拷贝和移动
    HttpClient(const HttpClient &) = delete;
    HttpClient &operator=(const HttpClient &) = delete;

private:
    static void die(const char *msg);
    static void log_error(const char *msg);
    static int32_t read_full(int fd, char *buf, size_t n);
    static int32_t write_all(int fd, const char *buf, size_t n);

    int m_fd = -1;
    static const size_t k_max_msg = 4096;
};
