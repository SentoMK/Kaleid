#include <httpserver.hpp>
#include <iostream>
#include <cstring>
HttpServer::HttpServer(uint16_t port) : m_port(port), m_listen_fd(-1)
{
    setup_server();
}

HttpServer::~HttpServer()
{
    if (m_listen_fd >= 0)
        close(m_listen_fd);
}

void HttpServer::start()
{
    listen_for_connections();
}

// 错误处理
void HttpServer::die(const char *msg)
{
    std::cerr << "[ERROR] " << msg << " (errno: " << errno << ")" << std::endl;
    exit(-1);
}
void HttpServer::log_error(const char *msg)
{
    std::cerr << "[ERROR] " << msg << std::endl;
}

// 初始化服务器socket
void HttpServer::setup_server()
{
    // socket()
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_fd < 0)
    {
        die("socket() failed");
    }
    int val = 1;
    setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in addr{
        addr.sin_family = AF_INET,
        addr.sin_port = htons(m_port),
        addr.sin_addr.s_addr = {htonl(INADDR_ANY)}};

    // bind()
    if (bind(m_listen_fd, (const sockaddr *)&addr, sizeof(addr)))
    {
        die("bind() failed");
    }

    // listen()
    if (listen(m_listen_fd, SOMAXCONN))
    {
        die("listen*() failed");
    }
}

// 监听并处理连接
void HttpServer::listen_for_connections()
{
    while (true)
    {
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(m_listen_fd, (sockaddr *)&client_addr, &addrlen);
        if (connfd < 0)
            continue;

        handle_client(connfd);
        close(connfd);
    }
}

// 处理单个客户端连接
void HttpServer::handle_client(int connfd)
{
    while (true)
    {
        if (process_request(connfd))
        {
            break; // 返回非0，终止连接
        }
    }
}

// 处理单个请求
int HttpServer::process_request(int connfd)
{
    char rbuf[4 + k_max_msg];
    uint32_t len = 0;

    // 读取消息头
    if (read_full(connfd, rbuf, 4))
    {
        log_error(errno == 0 ? "EOF" : "read() error");
        return -1;
    }

    memcpy(&len, rbuf, 4);
    if (len > k_max_msg)
    {
        log_error("Message too long");
        return -1;
    }

    // 读取消息体
    if (read_full(connfd, &rbuf[4], len))
    {
        log_error("read() error");
        return -1;
    }

    // 处理请求
    std::cout << "Client says: "
              << std::string(reinterpret_cast<const char *>(&rbuf[4]), len)
              << std::endl;

    // 发送响应
    const char reply[] = "Don't worry.";
    char wbuf[4 + sizeof(reply)];
    len = static_cast<uint32_t>(strlen(reply));
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], reply, len);

    return write_all(connfd, wbuf, 4 + len);
}

// 辅助函数：完整读取数据
int HttpServer::read_full(int fd, char *buf, size_t n)
{
    while (n > 0)
    {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0)
            return -1;
        n -= static_cast<size_t>(rv);
        buf += rv;
    }
    return 0;
}
// 辅助函数：完整写入数据
int HttpServer::write_all(int fd, char *buf, size_t n)
{
    while (n > 0)
    {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0)
            return -1;
        n -= static_cast<size_t>(rv);
        buf += rv;
    }
    return 0;
}
