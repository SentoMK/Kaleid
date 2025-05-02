#include <httpclient.hpp>
#include <iostream>
#include <cstring>

HttpClient::HttpClient(const char *host, uint16_t port)
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd < 0)
    {
        die("socket() failed");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);

    if (connect(m_fd, (const sockaddr *)&addr, sizeof(addr)))
    {
        die("connect() failed");
    }
}

HttpClient::~HttpClient()
{
    if (m_fd >= 0)
    {
        close(m_fd);
    }
}

void HttpClient::send_request(const std::string &msg)
{
    const uint32_t len = static_cast<uint32_t>(msg.size());
    if (len > k_max_msg)
    {
        die("Message too long");
    }

    char wbuf[4 + k_max_msg];
    memcpy(wbuf, &len, 4);
    memcpy(wbuf + 4, msg.data(), len);

    if (write_all(m_fd, wbuf, 4 + len))
    {
        die("write_all() failed");
    }
}

std::string HttpClient::receive_response()
{
    char header[4];
    if (read_full(m_fd, header, 4))
    {
        die("read header failed");
    }

    uint32_t len;
    memcpy(&len, header, 4);
    if (len > k_max_msg)
    {
        die("Response too long");
    }

    std::string resp(len, '\0');
    if (read_full(m_fd, &resp[0], len))
    {
        die("read body failed");
    }
    return resp;
}

// 错误处理
void HttpClient::die(const char *msg)
{
    std::cerr << "[ERROR] " << msg << " (errno: " << errno << ")" << std::endl;
    exit(-1);
}
void HttpClient::log_error(const char *msg)
{
    std::cerr << "[ERROR] " << msg << std::endl;
}

int32_t HttpClient::read_full(int fd, char *buf, size_t n)
{
    while (n > 0)
    {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0)
            return -1;
        n -= rv;
        buf += rv;
    }
    return 0;
}

int32_t HttpClient::write_all(int fd, const char *buf, size_t n)
{
    while (n > 0)
    {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0)
            return -1;
        n -= rv;
        buf += rv;
    }
    return 0;
}
