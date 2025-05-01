#include <httpserver.hpp>

int main()
{
    HttpServer server(1234);
    server.start();
    return 0;
}