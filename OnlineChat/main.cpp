// server_main.cpp
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <chrono>
#pragma comment(lib, "Ws2_32.lib")

#include "./Server/Server.h"   
#include "./Client/UserClient.h"


#define PORT 5555

int main()
{
//SERVER
    try 
    {
        sockets::server::Server server(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT, INADDR_ANY, SOMAXCONN);

        std::cout << "Starting TestServer on port " << PORT << std::endl;
        server.launch();

        std::cout << "Server running. Press <Enter> to stop." << std::endl;
        std::cin.get();

        server.stop();
    }
    catch (const std::exception& ex) 
    {
        std::cerr << "Exception in server: " << ex.what() << std::endl;
    }

    std::cout << "Server exited." << std::endl;
    return 0;

}
