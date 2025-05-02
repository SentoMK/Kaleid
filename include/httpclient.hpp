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
    explicit HttpClient(const std::string &host, uint16_t port);
    ~HttpClient();

    void connect();
    void send_request(const std::string &msg);
    std::string receive_response();

private:
    int m_sockfd = -1;
    std::string m_host;
    uint16_t m_port;

    void die(const char *msg) const;
};