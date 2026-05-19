#pragma once

#include <string>
#include <string_view>
#include <stdexcept>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

class IRSAWrapper 
{
public:
	virtual ~IRSAWrapper() = default;

	virtual bool generateRSAKeyPair() = 0;
	virtual std::string encrypt(std::string_view plainText) const = 0;
	virtual std::string decrypt(std::string_view cipherText) const = 0;
};

// container for RSA key pair and encryption/decryption methods
class RSAWrapper : public IRSAWrapper
{

public:
	constexpr static int DEFAULT_KEY_BIT_LENGTH = 2048;
	RSAWrapper(const RSAWrapper&) = delete;
	RSAWrapper& operator=(const RSAWrapper&) = delete;
	~RSAWrapper();
	 

	// key generation
	bool generateRSAKeyPair() override;

	// decryption and encryption
	std::string encrypt(std::string_view plainText) const override;
	std::string decrypt(std::string_view cipherText) const override;
	// getter for the pubkey
	std::string getPublicKey();

	// static encryptio and decryption
	static std::string encryptWithPublicKey(std::string_view plainText, std::string_view publicKey);
	static std::string decryptWithPrivateKey(std::string_view cipherText, std::string_view privateKey);

private:
	// holds key pair
	EVP_PKEY* pkey = nullptr;


};

