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
        try
        {
            in_addr addr{};
            if (InetPtonA(AF_INET, "127.0.0.1", &addr) != 1) {
                std::cerr << "inetptonA failed" << std::endl;
                return nullptr;
            }
            u_long serverIP = addr.S_un.S_addr;
            auto client = std::make_unique<Client::UserClient>(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT, serverIP);
            client->startClient();
            return client;

        }
        catch (const std::exception& e)
        {
            std::cerr << "Connection error: " << e.what() << std::endl;
            std::cerr << "Please check if the server is running and try again." << std::endl;
            exit(1);

        }

    }

    static int runClient(std::unique_ptr<Client::UserClient> client)
    {
        if (!client)
        {
            std::cerr << "Invalid client provided" << std::endl;
            return 1;
        }

        // simple menu loop for sending requests
        std::cout << "Please Enter UserName:" << std::endl;
        std::string username;
        std::cin >> username;

        // wait 5 seconds for server response for thet symmerytic key
        if(!client->sendPublicKey(50))
			throw Client::ClientException("failed to send public key to server, cannot continue");
        
        // automatically register and get the chat log
        client->registerUser(username);
        client->getChat();
        printHelp();
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
                    handleMessageChoice(client.get());
                }
                else if (choice == "/dm")
                {
					handleDirectMessageChoice(client.get());
                }
                else if (choice == "/reg")
                {
                    client->registerUser(username);
                }
				else if (choice == "/help")
				{
                    printHelp();
				}
				else if (choice == "/get")
				{
					client->getChat();
				}
                else
                    std::cout << "invalid option" << std::endl;

            }
            catch (const Client::ConnectionException& e)
            {
                std::cerr << "Connection error: " << e.what() << std::endl;
                std::cerr << "Please check if the server is running and try again." << std::endl;
            }

            catch (const Client::ClientException& e)
            {
                std::cerr << "Client error: " << e.what() << std::endl;
                std::cerr << "An unexpected error occurred. Please try again." << std::endl;
            }

        }
        return 0;
    }
    static void printHelp()
    {
        std::cout << "Choose Request Type:" << std::endl;
        std::cout << "  (/reg) register with the username entered at startup" << std::endl;
        std::cout << "  (/msg <message>) Send Message" << std::endl;
        std::cout << "  (/dm <message> <recver username>) Direct Message" << std::endl;
		std::cout << "  (/get) get chat log" << std::endl;
        std::cout << "  (/help) for help" << std::endl;
        std::cout << "  q) quit" << std::endl;
    }

    static void handleMessageChoice(Client::UserClient* client)
    {
        std::string msg;
        std::cin >> msg;

        if (msg.empty())
        {
            std::cout << "msg empty cant send" << std::endl;
            return;
        }

        client->sendMessage(msg);
    }

    static void handleDirectMessageChoice(Client::UserClient* client)
    {
        std::string msg;
        std::cin >> msg;

        std::string recver;
        std::cin >> recver;

        if (msg.empty() || recver.empty())
        {
            std::cout << "message or recver is empty, direct message cancelled" << std::endl;
            return;
        }

        std::cout << "sending msg to user: " << recver << "..." << std::endl;
        client->sendDirectMessage(msg, recver);
    }

    static void handleRegisterChoice(Client::UserClient* client, const std::string& username)
    {
        if (username.empty())
        {
            std::cout << "username is empty, registration cancelled" << std::endl;
            return;
        }

        client->registerUser(username);
    }
};



int runServer()
{
    try
    {
        sockets::server::Server server(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT, INADDR_ANY, SOMAXCONN);

        std::cout << "Starting on port " << PORT << std::endl;

        server.launch();

        std::cout << "Server running. Press <Enter> to stop." << std::endl;

        std::cin.get();

        server.stop();
        return 0;
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

}
