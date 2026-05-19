#include "RSAWrapper.h"

RSAWrapper::~RSAWrapper()
{
    if(pkey)
        EVP_PKEY_free(pkey);
}

bool RSAWrapper::generateRSAKeyPair() 
{
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx) return false;

    if (EVP_PKEY_keygen_init(ctx) <= 0 ||
        EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, DEFAULT_KEY_BIT_LENGTH) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    EVP_PKEY* newKey = nullptr;
    bool success = (EVP_PKEY_keygen(ctx, &newKey) > 0);

    EVP_PKEY_CTX_free(ctx);

    if (!success) {
        if (newKey) EVP_PKEY_free(newKey);
        return false;
    }

    if (pkey) EVP_PKEY_free(pkey);
    pkey = newKey;
    return true;
}


std::string RSAWrapper::encrypt(std::string_view plainText) const
{
    if (!pkey || plainText.empty()) return "";
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) return "";

    // init for encryption
    if (EVP_PKEY_encrypt_init(ctx) <= 0 ||
        EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return "";
    }

	// checks the size of the output buffer needed for encryption
    size_t out_len = 0;
    const unsigned char* in_data = reinterpret_cast<const unsigned char*>(plainText.data());
    if (EVP_PKEY_encrypt(ctx, nullptr, &out_len, in_data, plainText.size()) <= 0) 
    {
        EVP_PKEY_CTX_free(ctx);
        return "";
    }

    // actual encryption
    std::string ciphertext(out_len, '\0');
    unsigned char* out_data = reinterpret_cast<unsigned char*>(ciphertext.data());
    if (EVP_PKEY_encrypt(ctx, out_data, &out_len, in_data, plainText.size()) <= 0) 
    {
        EVP_PKEY_CTX_free(ctx);
        return "";
    }

    ciphertext.resize(out_len);
    EVP_PKEY_CTX_free(ctx);
    return ciphertext;
}

std::string RSAWrapper::encryptWithPublicKey(std::string_view plainText, std::string_view publicKey)
{
    if (publicKey.empty() || plainText.empty()) return "";

    // create a local evp
    BIO* bio = BIO_new_mem_buf(publicKey.data(), static_cast<int>(publicKey.size()));
    if (!bio) return "";
    EVP_PKEY* local_pub_key = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!local_pub_key) return "";

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(local_pub_key, nullptr);
    if (!ctx) {
        EVP_PKEY_free(local_pub_key);
        return "";
    }

    // init padding
    if (EVP_PKEY_encrypt_init(ctx) <= 0 ||
        EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(local_pub_key);
        return "";
    }

    // check length
    size_t out_len = 0;
    const unsigned char* in_data = reinterpret_cast<const unsigned char*>(plainText.data());

    if (EVP_PKEY_encrypt(ctx, nullptr, &out_len, in_data, plainText.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(local_pub_key);
        return "";
    }

	// get ciphertext
    std::string ciphertext(out_len, '\0');
    unsigned char* out_data = reinterpret_cast<unsigned char*>(ciphertext.data());
    if (EVP_PKEY_encrypt(ctx, out_data, &out_len, in_data, plainText.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(local_pub_key);
        return "";
    }

    ciphertext.resize(out_len);
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(local_pub_key); 

    return ciphertext;
}

std::string RSAWrapper::decryptWithPrivateKey(std::string_view cipherText, std::string_view privateKey)
{
    // currently uneeded
    throw std::logic_error("decryptWithPrivateKey is not implemented");

}



std::string RSAWrapper::decrypt(std::string_view cipherText) const
{
    if (!pkey || cipherText.empty()) return "";

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) return "";

    // match padding
    if (EVP_PKEY_decrypt_init(ctx) <= 0 ||
        EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return "";
    }

    size_t out_len = 0;
    const unsigned char* in_data = reinterpret_cast<const unsigned char*>(cipherText.data());

    // determine size of output
    if (EVP_PKEY_decrypt(ctx, nullptr, &out_len, in_data, cipherText.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return "";
    }

    std::string plainText(out_len, '\0');
    unsigned char* out_data = reinterpret_cast<unsigned char*>(plainText.data());

    // actual decrytion
    if (EVP_PKEY_decrypt(ctx, out_data, &out_len, in_data, cipherText.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return "";
    }
    plainText.resize(out_len);
    EVP_PKEY_CTX_free(ctx);

    return plainText;
}

std::string RSAWrapper::getPublicKey()
{
    if (!pkey)
        return "";

    BIO* bio = BIO_new(BIO_s_mem());
    if (!bio)
        return "";

    if (PEM_write_bio_PUBKEY(bio, pkey) != 1)
    {
        BIO_free(bio);
        return "";
    }

    BUF_MEM* buffer = nullptr;
    BIO_get_mem_ptr(bio, &buffer);

    if (!buffer || !buffer->data || buffer->length == 0)
    {
        BIO_free(bio);
        return "";
    }

    std::string publicKey(buffer->data, buffer->length);

    BIO_free(bio);
    return publicKey;
}