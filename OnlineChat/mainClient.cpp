#include <iostream>
#include <string>
#include <limits>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "./Client/UserClient.h"
#define PORT    5555


int main()
{
    std::cout << "starting client..." << std::endl;

    // convert "127.0.0.1" to an in_addr
    in_addr addr{};
    if (InetPtonA(AF_INET, "127.0.0.1", &addr) != 1)
    {
        std::cerr << "inetptonA failed" << std::endl;
        return 1;
    }

    u_long serverIP = addr.S_un.S_addr;
    std::cout << "server ip (raw): " << serverIP << std::endl;

    // create the client object and connect to the server
    Client::UserClient client(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT, serverIP);

    // simple menu loop for sending requests
    while (true)
    {
        std::cout << std::endl;
        std::cout << "choose request type:" << std::endl;
        std::cout << "  1) getchat (get full chat)" << std::endl;
        std::cout << "  2) sendmessage (send a message)" << std::endl;
        std::cout << "  0) quit" << std::endl;
        std::cout << "> " << std::flush;

        int choice = -1;
        if (!(std::cin >> choice))
        {
            // input stream is broken (e.g. ctrl+z), stop the client
            std::cerr << "input error, exiting" << std::endl;
            return 1;
        }



        if (choice == 0)
        {
            std::cout << "goodbye" << std::endl;
            break;
        }

        switch (choice)
        {
        case 1: // getchat
        {
            std::cout << "sending getchat request..." << std::endl;
            // request type 1, empty body
            client.sendPacket("", 1);
            // receive and handle server response
            client.recievePacket();
            break;
        }
        case 2: // sendmessage
        {
            std::cout << "enter message to send: " << std::flush;
            std::string msg;
            std::getline(std::cin, msg);

            if (msg.empty())
            {
                std::cout << "message is empty, nothing sent" << std::endl;
                break;
            }

            std::cout << "sending message..." << std::endl;
            // request type 2, message as body
            client.sendPacket(msg.c_str(), 2);
            // receive and handle server response (if your protocol expects one)
            client.recievePacket();
            break;
        }
        default:
        {
            std::cout << "invalid option, choose 0, 1,  2" << std::endl;
            break;
        }
        }
    }

    return 0;
}









