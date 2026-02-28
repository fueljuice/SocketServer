#pragma once

#include <stdexcept>
#include <string>

namespace Client
{

// base exception for all client related errors
class ClientException : public std::runtime_error
{
public:
    explicit ClientException(const std::string& message) 
        : std::runtime_error(message) {}
};

// base exception for protocol related errors
class ProtocolException : public ClientException
{
public:
    explicit ProtocolException(const std::string& message)
        : ClientException("protocol error: " + message) {}
};

// base exception for network related errors
class NetworkException : public ClientException
{
public:
    explicit NetworkException(const std::string& message)
        : ClientException("network error: " + message) {}
};

// thrown when server sends malformed or invalid headers
class InvalidHeaderException : public ProtocolException
{
public:
    explicit InvalidHeaderException(const std::string& details = "")
        : ProtocolException("invalid server header" + (details.empty() ? "" : ": " + details)) {}
};

// thrown when server response doesnt match expected format
class InvalidResponseException : public ProtocolException
{
public:
    explicit InvalidResponseException(const std::string& details = "")
        : ProtocolException("invalid server response" + (details.empty() ? "" : ": " + details)) {}
};

// thrown when network connection fails
class ConnectionException : public NetworkException
{
public:
    explicit ConnectionException(const std::string& details = "")
        : NetworkException("connection failed" + (details.empty() ? "" : ": " + details)) {}
};

// thrown when received data is corrupted
class DataCorruptionException : public ProtocolException
{
public:
    explicit DataCorruptionException(const std::string& details = "")
        : ProtocolException("data corruption detected" + (details.empty() ? "" : ": " + details)) {}
};
}
