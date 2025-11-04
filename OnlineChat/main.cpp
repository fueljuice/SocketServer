// server_main.cpp
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include "TestServer.h"   // your header that declares HDE::TestServer

int main(int argc, char** argv)
{
    int port = 5555;
    if (argc >= 2) 
    {
        port = std::atoi(argv[1]);
    }


    try 
    {
        HDE::TestServer server(AF_INET, SOCK_STREAM, IPPROTO_TCP, port, INADDR_ANY, SOMAXCONN);

        std::cout << "Starting TestServer on port " << port << "...\n";
        server.launch();

        std::cout << "Server running. Press <Enter> to stop.\n";
        std::cin.get();

        server.stop();
    }
    catch (const std::exception& ex) 
    {
        std::cerr << "Exception in server: " << ex.what() << "\n";
    }

    std::cout << "Server exited.\n";
    return 0;
}