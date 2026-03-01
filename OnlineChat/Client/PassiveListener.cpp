#include "PassiveListener.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif

Client::PassiveListener::PassiveListener(ResponseReader& responseReader_p, INetworkManager& net_p)
    :
    reader(responseReader_p),
    net(net_p)
{
}

void Client::PassiveListener::startPassiveListener()
{
    // ceheck if already listening
    if (shouldListen.load())
        return;

    shouldListen.store(true);
    listenerThread = std::thread(&PassiveListener::passiveListenLoop, this);
}

void Client::PassiveListener::stopPassiveListener()
{
    shouldListen.store(false);
    if (listenerThread.joinable())
        listenerThread.join();
}

bool Client::PassiveListener::isListening()
{
    return shouldListen;
}

void Client::PassiveListener::passiveListenLoop()
{
    try
    {
        while (shouldListen.load())
        {
            if (checkForMessages())
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            else
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    catch (const Client::ClientException& e)
    {
        std::cerr << "exeception in listener: " << e.what() << std::endl;
        shouldListen.store(false);
    }
}

bool Client::PassiveListener::checkForMessages()
{
    if (net.isSocketClosed())
        throw ConnectionException("server closed");

    SOCKET sock = net.getSock();

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sock, &readSet);
    timeval timeout{ 1, 0 };

    int result = select(0, &readSet, nullptr, nullptr, &timeout);
    if (result <= 0) return false;
    DBG("select:" << result);
    if (FD_ISSET(sock, &readSet))
    {

        DBG("found msg");
        auto resp = reader.readResponse();
        if (!resp)
            throw InvalidResponseException("passive reader got a bad response from server");
        std::cout << "[NEW MESSAGE]: " << *resp << std::endl;
        return true;
    }
    return false;
}
