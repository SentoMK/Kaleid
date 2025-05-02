#include <httpclient.hpp>
#include <iostream>
#include <cstring>

HttpClient::HttpClient(const std::string &host, uint16_t port)
    : m_host(host), m_port(port) {}

HttpClient::~HttpClient()
{
    if (m_sockfd >= 0)
        close(m_sockfd);
}

void HttpClient::connect()
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0)
    {
        die("socket() failed");
    }

    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_port);
    if (inet_pton(AF_INET, m_host.c_str(), &server_addr.sin_addr) <= 0)
    {
        die("inet_pton failed");
    }

    if (::connect(m_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        die("connect() failed");
    }
}

void HttpClient::send_request(const std::string &msg)
{
    uint32_t len = msg.size();
    char buf[4 + len];
    memcpy(buf, &len, 4);
    memcpy(buf + 4, msg.data(), len);
    if (write(m_sockfd, buf, sizeof(buf)) < 0)
    {
        die("write() failed");
    }
}

std::string HttpClient::receive_response()
{
    char header[4];
    if (read(m_sockfd, header, 4) != 4)
        die("read header failed");
    uint32_t len;
    memcpy(&len, header, 4);
    std::string resp(len, '\0');
    if (read(m_sockfd, &resp[0], len) != len)
        die("read body failed");
    return resp;
}

// 错误处理
void HttpClient::die(const char *msg) const
{
    std::cerr << "[Client Error] " << msg << std::endl;
    exit(EXIT_FAILURE);
}