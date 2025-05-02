#include <httpclient.hpp>
#include <iostream>

int main()
{
    try
    {
        HttpClient client("127.0.0.1", 1234);

        // 发送多个请求
        client.send_request("hello1");
        std::cout << "Response 1: " << client.receive_response() << std::endl;

        client.send_request("hello2");
        std::cout << "Response 2: " << client.receive_response() << std::endl;

        client.send_request("hello3");
        std::cout << "Response 3: " << client.receive_response() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Fatal error occurred" << std::endl;
        return 1;
    }
    return 0;
}
