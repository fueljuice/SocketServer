#include "./Client/UserClient.h"
#include "./Client/ClientExceptions.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <memory>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 5555

class ClientManager
{
public:
    static std::unique_ptr<Client::UserClient> createClient()
    {
        std::cout << "starting client..." << std::endl;

        // convert "127.0.0.1" to an in_addr
        in_addr addr{};
        if (InetPtonA(AF_INET, "127.0.0.1", &addr) != 1) {
            std::cerr << "inetptonA failed" << std::endl;
            return nullptr;
        }

        u_long serverIP = addr.S_un.S_addr;
        std::cout << "server ip (raw): " << serverIP << std::endl;

        // create the client object and connect to the server
        auto client = std::make_unique<Client::UserClient>(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT, serverIP);

        // start passive listening for incoming messages
        client->startPassiveListener();

        return client;
    }

    static int runClient(std::unique_ptr<Client::UserClient> client)
    {
        if (!client)
        {
            std::cerr << "Invalid client provided" << std::endl;
            return 1;
        }

        const char* name = "userName";

        try
        {
            // simple menu loop for sending requests
            client->sendRequest("", "", messaging::ActionType::GET_CHAT);
            std::cout << "response:" << client->recieveResponse() << std::endl;
            std::cout << std::endl;
            std::cout << "choose request type:" << std::endl;
            std::cout << "  (/msg) send message" << std::endl;
            std::cout << "  (/reg) register " << std::endl;
            std::cout << "  (/dm) register " << std::endl;
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
                        std::cout << "enter username to register: ";
                        std::string msg;
                        std::cin >> msg;
                        if (msg.empty())
                            std::cout << "username is empty, registration cancelled" << std::endl;
                        // std::cout << "registering user " << msg << "..." << std::endl;
                        client->sendRequest(msg.c_str(), "", messaging::ActionType::SEND_MESSAGE);
                    }
                    else if (choice == "/reg")
                    {
                        std::cout << "enter username to register: ";
                        std::string username;
                        std::cin >> username;
                        if (username.empty())
                            std::cout << "username is empty, registration cancelled" << std::endl;
                        std::cout << "registering user " << username << "..." << std::endl;
                        client->sendRequest(username.c_str(), "", messaging::ActionType::REGISTER);
                    }
                    else if (choice == "/dm")
                    {
                        std::cout << "enter username to send a DM: ";
                        std::string username;
                        std::string recver;
                        std::cin >> username;
                        std::cin >> recver;
                        if (username.empty() || recver.empty())
                        {
                            std::cout << "username or recver is empty, registration cancelled" << std::endl;
                        }
                        std::cout << "sending msg to user: " << username << "..." << std::endl;
                        client->sendRequest("MSGTEST", recver.c_str(), messaging::ActionType::DIRECT_MESSAGE);
                    }
                    else
                        std::cout << "invalid option, choose 0, 1 or 2" << std::endl;


                }
                catch (const Client::ConnectionException& e)
                {
                    std::cerr << "Connection error: " << e.what() << std::endl;
                    std::cerr << "Please check if the server is running and try again." << std::endl;
                    return 1;
                }

                catch (const Client::ProtocolException& e)
                {
                    std::cerr << "Protocol error: " << e.what() << std::endl;
                    std::cerr << "The server may be using an incompatible protocol." << std::endl;
                }
                catch (const Client::DataCorruptionException& e)
                {
                    std::cerr << "Data corruption error: " << e.what() << std::endl;
                    std::cerr << "Please try the request again." << std::endl;
                }
                catch (const Client::ClientException& e)
                {
                    std::cerr << "Client error: " << e.what() << std::endl;
                    std::cerr << "An unexpected error occurred. Please try again." << std::endl;
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Unexpected error: " << e.what() << std::endl;
            return 1;
        }
        std::cout << "shutting down client..." << std::endl;
        return 0;
    }
};

int mainClient()
{
    auto client = ClientManager::createClient();
    if (!client) {
        return 1;
    }
    std::cout << "client started successfully" << std::endl;
    return ClientManager::runClient(std::move(client));
}
