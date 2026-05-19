#include "OpenSSLWrapper.h"
#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif
OpenSSLWrapper::OpenSSLWrapper(bool isServer)
{
	if (isServer)
		initServerCTX();
}

OpenSSLWrapper::~OpenSSLWrapper()
{
}

bool OpenSSLWrapper::sslSend(SOCKET s, std::string_view msg, size_t len)
{
	SSL* ssl;
	ssl = SSL_new(context);
	SSL_set_fd(ssl, static_cast<int>(s));
	return true;
}


bool OpenSSLWrapper::initClientCTX()
{
	return false;
}

bool OpenSSLWrapper::initServerCTX()
{
	// create a context
	context = SSL_CTX_new(TLS_server_method());
	if (!context)
		return false;
	// make it use a crt
	if (SSL_CTX_use_certificate_file(context, "./security/broker.crt", SSL_FILETYPE_PEM) <= 0)
	{
		DBG("can't load certificate from the file system...");
		context = nullptr;
		return false;

	}
	// make it use a privatekey
	if (SSL_CTX_use_PrivateKey_file(context, "./security/broker.key", SSL_FILETYPE_PEM) <= 0)
	{
		DBG("can't load private key from the file system...");
		return false;
	}
	return true;
}
