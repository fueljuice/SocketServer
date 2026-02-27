#include "ClientThreadManager.h"

void sockets::server::ClientThreadManager::joinAll()
{
	for (auto& t : threads_) if (t.joinable()) t.join();
}
