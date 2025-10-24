
#include "ListeningSocket.h"
#include "TestServer.h"
//#pragma comment(lib, "Ws2_32.lib")
int main()
{
	//std::cout << "start" << std::endl;
	//std::cout << "bindingsocket" << std::endl;
	//HDE::BindingSocket bs = HDE::BindingSocket(AF_INET, SOCK_STREAM, 0, 8881, INADDR_ANY);
	//std::cout << "listen" << std::endl;
	//HDE::ListeningSocket ls = HDE::ListeningSocket(AF_INET, SOCK_STREAM, 0, 8880, INADDR_ANY, 10);
	//std::cout << "sucses" << std::endl;

	HDE::TestServer tstserver(AF_INET, SOCK_STREAM, 0, 8880, INADDR_ANY, 10);
}