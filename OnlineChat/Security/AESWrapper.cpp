#include "AESWrapper.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif


std::optional<std::string> AESWrapper::decryptWithKey(std::string_view ciphertext, std::string_view key)
{
	DBG("decrypting with key");
    if (key.size() != AES_KEY_SIZE || ciphertext.size() < AES_IV_SIZE) 
		return {};
    DBG("decrypting with key2");

    const auto* iv = reinterpret_cast<const unsigned char*>(ciphertext.data());

    const auto* encrypted =
        reinterpret_cast<const unsigned char*>(ciphertext.data() + AES_IV_SIZE);

    const int encrypted_len =
        static_cast<int>(ciphertext.size() - AES_IV_SIZE);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        return {};

    DBG("decrypting with key3");

    std::string decrypted;
    decrypted.resize(encrypted_len + EVP_MAX_BLOCK_LENGTH);

    int len = 0;
    int plaintext_len = 0;

    bool ok =
        EVP_DecryptInit_ex(
            ctx,
            EVP_aes_256_cbc(),
            nullptr,
            reinterpret_cast<const unsigned char*>(key.data()),
            iv) == 1
        &&
        EVP_DecryptUpdate(
            ctx,
            reinterpret_cast<unsigned char*>(decrypted.data()),
            &len,
            encrypted,
            encrypted_len) == 1;

    if (ok) {
        plaintext_len = len;

        ok =
            EVP_DecryptFinal_ex(
                ctx,
                reinterpret_cast<unsigned char*>(decrypted.data()) + len,
                &len) == 1;

        if (ok) {
            plaintext_len += len;
        }
    }

    EVP_CIPHER_CTX_free(ctx);
    DBG("decrypting with key4");

    if (!ok) {
        return {};
    }

    decrypted.resize(plaintext_len);
    DBG("returning final decryption5");
    return decrypted;
}


std::optional<std::string> AESWrapper::generateAESKey()
{
    std::string key;
    key.resize(AES_KEY_SIZE);

	// generates random bytes for the AES key
    if (RAND_bytes(reinterpret_cast<unsigned char*>(key.data()), AES_KEY_SIZE) != 1)
        return std::nullopt;

    return key;
}

std::optional<std::string>  AESWrapper::encrypt(std::string_view plainText) const
{
    return encryptWithKey(plainText, key_);
}

std::optional<std::string>  AESWrapper::decrypt(std::string_view cipherText) const
{
    return decryptWithKey(cipherText, key_);
}

void AESWrapper::setKey(std::string key)
{
	key_ = std::move(key);
}

std::string AESWrapper::getKey() const
{
	return key_;
}

bool AESWrapper::hasKey() const
{
	return !key_.empty();
}

std::optional<std::string>  AESWrapper::encryptWithKey(std::string_view plaintext, std::string_view key)
{
    if (key.size() != AES_KEY_SIZE)
        return {}; 

    std::string result;
    result.resize(AES_IV_SIZE + plaintext.size() + EVP_MAX_BLOCK_LENGTH);

    auto* iv = reinterpret_cast<unsigned char*>(result.data());

    if (RAND_bytes(iv, AES_IV_SIZE) != 1) {
        return {};
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return {};
    }

    int len = 0;
    int ciphertext_len = 0;

    bool ok =
        EVP_EncryptInit_ex(
            ctx,
            EVP_aes_256_cbc(),
            nullptr,
            reinterpret_cast<const unsigned char*>(key.data()),
            iv) == 1
        &&
        EVP_EncryptUpdate(
            ctx,
            reinterpret_cast<unsigned char*>(result.data()) + AES_IV_SIZE,
            &len,
            reinterpret_cast<const unsigned char*>(plaintext.data()),
            static_cast<int>(plaintext.size())) == 1;

    if (ok) {
        ciphertext_len = len;

        ok =
            EVP_EncryptFinal_ex(
                ctx,
                reinterpret_cast<unsigned char*>(result.data()) + AES_IV_SIZE + len,
                &len) == 1;

        if (ok) {
            ciphertext_len += len;
        }
    }

    EVP_CIPHER_CTX_free(ctx);

    if (!ok)
        return {};

    result.resize(AES_IV_SIZE + ciphertext_len);
    return result;
}