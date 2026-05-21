// server_main.cpp

#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <chrono>
#include <string>
#include <memory>
#include <optional>

#pragma comment(lib, "Ws2_32.lib")

#include "./Server/Server.h"   
#include "./Client/UserClient.h"
#include "./Client/ClientExceptions.h"
#include "./Sockets/ConnectingSocket.h"

#define PORT 5555

class ClientManager
{
public:
    static std::unique_ptr<Client::UserClient> createClient()
    {
        std::cout << "starting client..." << std::endl;
        in_addr addr{};
        if (InetPtonA(AF_INET, "127.0.0.1", &addr) != 1) {
            std::cerr << "inetptonA failed" << std::endl;
            return nullptr;
        }
        u_long serverIP = addr.S_un.S_addr;
        std::cout << "server ip (raw): " << serverIP << std::endl;
        auto client = std::make_unique<Client::UserClient>(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT, serverIP);
        client->startClient();

        return client;
    }

    static int runClient(std::unique_ptr<Client::UserClient> client)
    {
        if (!client)
        {
            std::cerr << "Invalid client provided" << std::endl;
            return 1;
        }

        // simple menu loop for sending requests
        std::cout << "Register To Start The Chat... Please Enter UserName:" << std::endl;
        std::string username;
        std::cin >> username;
        client->sendPublicKey();

        std::cout << std::endl;
        std::cout << "Choose Request Type:" << std::endl;
        std::cout << "  (/reg <username>) register:" << std::endl;
        std::cout << "  (/msg <message>) Send Message:" << std::endl;
        std::cout << "  (/dm <message> <recver username>) Direct Message: " << std::endl;
        std::cout << "  (/help) for help: " << std::endl;
        std::cout << "  q) quit" << std::endl;
        while (true)
        {

            std::string choice = "";
            if (!(std::cin >> choice))
            {
                std::cerr << "input error, exiting" << std::endl;
                return -1;
            }

            if (choice == "q")
            {
                std::cout << "goodbye" << std::endl;
                break;
            }

            try
            {

                if (choice == "/msg")
                {
                    std::string msg;
                    std::cin >> msg;
                    if (msg.empty())
                        std::cout << "msg empty cant send" << std::endl;
                    client->sendToServer(msg, "", messaging::RequestType::SEND_MESSAGE);
                }
                else if (choice == "/dm")
                {
                    std::string msg;
                    std::cin >> msg;
                    std::string recver;
                    std::cin >> recver;
                    if (username.empty() || recver.empty())
                    {
                        std::cout << "username or recver is empty, registration cancelled" << std::endl;
                    }
                    std::cout << "sending msg to user: " << recver << "..." << std::endl;
                    client->sendToServer(msg, recver, messaging::RequestType::DIRECT_MESSAGE);
                }
                else if (choice == "/reg")
                {
                    std::string username;
                    std::cin >> username;
                    client->sendToServer(username, messaging::RequestType::REGISTER);
                    client->sendToServer("", messaging::RequestType::GET_CHAT);

                }
				else if (choice == "/help")
				{
					std::cout << "Choose Request Type:" << std::endl;
					std::cout << "  (/reg <username>) register:" << std::endl;
					std::cout << "  (/msg <message>) Send Message:" << std::endl;
					std::cout << "  (/dm <message> <recver username>) Direct Message: " << std::endl;
					std::cout << "  (/help) for help: " << std::endl;
					std::cout << "  q) quit" << std::endl;
				}
                else
                    std::cout << "invalid option" << std::endl;


            }
            catch (const Client::ConnectionException& e)
            {
                std::cerr << "Connection error: " << e.what() << std::endl;
                std::cerr << "Please check if the server is running and try again." << std::endl;
                return 1;
            }

            catch (const Client::ClientException& e)
            {
                std::cerr << "Client error: " << e.what() << std::endl;
                std::cerr << "An unexpected error occurred. Please try again." << std::endl;
            }
        }
        return 0;
    }
};

void showUsage(const char* programName)
{
    std::cout << "Usage: " << programName << " [mode]" << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  server - Run the chat server (default)" << std::endl;
    std::cout << "  client - Run the chat client" << std::endl;
}

int runServer()
{
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
        return 1;
    }

    std::cout << "Server exited." << std::endl;
    return 0;
}

int runClient()
{
    auto client = ClientManager::createClient();
    if (!client) {
        return 1;
    }
    std::cout << "client started successfully" << std::endl;
    return ClientManager::runClient(std::move(client));
}

int main(int argc, char* argv[])
{
    std::string mode = "server"; // default mode

    if (argc > 1)
    {
        mode = argv[1];
    }

    if (mode == "server")
    {
        std::cout << "Starting in SERVER mode..." << std::endl;
        return runServer();
    }
    else if (mode == "client")
    {
        std::cout << "Starting in CLIENT mode..." << std::endl;
        return runClient();
    }
    else
    {
        showUsage(argv[0]);
        return 1;
    }
}
