#include "NetworkIO.h"

bool sockets::server::NetworkIO::sendAll(SOCKET s, std::string_view payload)
{
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

std::string sockets::server::NetworkIO::recvAll(SOCKET sock, size_t size)
{
    std::string data;
    data.resize(size);
    size_t lengthHeaderBytes = recv(
        sock,
        data.data(),
        size, 
        MSG_WAITALL);
	return data;
}
