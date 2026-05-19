#pragma once
#include <string>
#include <optional>
#include <string_view>

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rand.h>

class IAESWrapper
{
public:
    virtual ~IAESWrapper() = default;

    virtual std::string encrypt(std::string_view plainText) const = 0;
    virtual std::string decrypt(std::string_view cipherText) const = 0;

    virtual void setKey(std::string key) = 0;
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
    std::string encrypt(std::string_view plainText) const override;
    std::string decrypt(std::string_view cipherText) const override;

    // set the key after construction
    void setKey(std::string key) override;

    // decrypt and encrypt without constructing the class
    static std::string encryptWithPublicKey(std::string_view plainText, std::string_view publicKey);
    static std::string decryptWithPrivateKey(std::string_view cipherText, std::string_view privateKey);

private:
    std::string key_;
};