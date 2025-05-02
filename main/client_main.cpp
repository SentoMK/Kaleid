#include <httpclient.hpp>
#include <iostream>
#include <thread>
#include <vector>

void client_task(int client_id)
{
    try
    {
        HttpClient client("127.0.0.1", 1234);
        client.connect();
        client.send_request("hello from client " + std::to_string(client_id));
        std::string response = client.receive_response();
        std::cout << "Client " << client_id
                  << " received: " << response << std::endl;
    }
    catch (...)
    {
        std::cerr << "Client " << client_id << " failed." << std::endl;
    }
}

int main()
{
    const int num_clients = 5; 
    std::vector<std::thread> threads;

    for (int i = 0; i < num_clients; ++i)
    {
        threads.emplace_back(client_task, i + 1); // 传递客户端ID
    }

    for (auto &t : threads)
    {
        t.join();
    }

    return 0;
}
