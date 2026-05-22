#include "PassiveListener.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif

Client::PassiveListener::PassiveListener(IResponseReader& responseReader_p, INetworkManager& net_p, IResponseHandler& gui_p)
    :
    reader(responseReader_p),
    net(net_p),
    handler(gui_p)
    {}

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
		// loop until stop signal is given, check for messages every CHECK_INTERVAL
        while (shouldListen.load())
        {
            if (checkForMessages())
                std::this_thread::sleep_for(CHECK_INTERVAL);
            else
                std::this_thread::sleep_for(CHECK_INTERVAL);
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
	DBG("checking for messages...");
    SOCKET sock = net.getSock();

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sock, &readSet);
    timeval timeout{ 1, 0 };
    // check if packet found synchnusly using select
    int result = select(0, &readSet, nullptr, nullptr, &timeout);
    if (result <= 0) return false;
    DBG("select:" << result);
    if (FD_ISSET(sock, &readSet))
    {
		// packet found. read it and route it to the handler
        DBG("found msg");
        const auto resp = reader.readResponse();
        if (!resp)
            throw InvalidResponseException("passive reader got a bad response from server");
        handler.handleResponse(resp->first, resp->second);
        return true;
    }
    return false;
}
