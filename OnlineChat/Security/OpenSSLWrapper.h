#pragma once
#include <openssl\ssl.h>
#include <winsock.h>
#include <iostream>
#include <string_view>
class OpenSSLWrapper
{

public:
	OpenSSLWrapper(bool isServer);
	~OpenSSLWrapper();
	bool sslSend(SOCKET s, std::string_view msg, size_t len);
	bool sslread(SOCKET s, std::string_view msg, size_t len);
	
private:
	bool initClientCTX();
	bool initServerCTX();
	SSL_CTX* context;
};

