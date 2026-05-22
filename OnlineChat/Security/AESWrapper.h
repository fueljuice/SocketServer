#pragma once
#include <string>
#include <optional>
#include <string_view>
#include <iostream>

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rand.h>

class IAESWrapper
{
public:
    virtual ~IAESWrapper() = default;

    virtual std::optional<std::string> encrypt(std::string_view plainText) const = 0;
    virtual std::optional<std::string> decrypt(std::string_view cipherText) const = 0;

    virtual void setKey(std::string key) = 0;
    virtual std::string getKey() const = 0;
	virtual bool hasKey() const = 0;
};


// conatainer for AES key and encryption/decryption methods
class AESWrapper final : public IAESWrapper
{
public:
    static constexpr size_t AES_KEY_SIZE = 32; // AES-256
    static constexpr size_t AES_IV_SIZE = 16;  // AES-CBC IV size

    AESWrapper() = default;

    // avoid duplicating key container
    AESWrapper(const AESWrapper&) = delete;
    AESWrapper& operator=(const AESWrapper&) = delete;

    AESWrapper(AESWrapper&&) noexcept = default;
    AESWrapper& operator=(AESWrapper&&) noexcept = default;

    // key generation
    static std::optional<std::string> generateAESKey();

    // IAESWrapper implementation
    std::optional<std::string>  encrypt(std::string_view plainText) const override;
    std::optional<std::string>  decrypt(std::string_view cipherText) const override;

    // setter for the key after construction
    void setKey(std::string key) override;
    std::string getKey() const;
    virtual bool hasKey() const override;
    // decrypt and encrypt without constructing the class
    static std::optional<std::string>  encryptWithKey(std::string_view plainText, std::string_view publicKey);
    static std::optional<std::string>  decryptWithKey(std::string_view cipherText, std::string_view privateKey);

private:
    std::string key_;
};