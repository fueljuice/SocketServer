#include "NetworkIO.h"
#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG
bool sockets::server::NetworkIO::sendAll(SOCKET s, std::string_view payload)
{
	DBG("sending: ");
    std::lock_guard<std::mutex> lk(sendMutex);
    const std::size_t len = payload.size();
    int sent = 0, r;

    while (sent < len)
    {
        r = send(s, payload.data() + sent, len - sent, 0);
        if (r <= 0)
            return false;
        sent += r;
    }
    return true;
}

std::optional<std::string> sockets::server::NetworkIO::recvAll(SOCKET sock, size_t size)
{
    std::string data;
    data.resize(size);
    size_t lengthHeaderBytes = recv(
        sock,
        data.data(),
        static_cast<int>(size), 
        MSG_WAITALL);
    // verfies everything was sent
    if (size != lengthHeaderBytes)
        return std::nullopt;
	return data;
}
