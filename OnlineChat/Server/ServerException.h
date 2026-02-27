#pragma once
// server_exceptions.h
//
// minimal exception types for a socket server.
// throw these from lower layers (protocol/storage/network/etc.).
// catch at the request/connection boundary and translate into a status::error response.

#include <stdexcept>
#include <string>
#include <utility>

namespace sockets::server 
{

/// base error for all server exceptions.
class ServerError : public std::runtime_error 
{
public:
    explicit ServerError(std::string message)
        : std::runtime_error(std::move(message)) {}
};

/// thrown when a request/response cannot be parsed/validated.
class ProtocolError final : public ServerError 
{
public:
    explicit ProtocolError(std::string message)
        : ServerError(std::move(message)) {}
};

/// thrown for network send/recv/accept failures.
class NetworkError final : public ServerError 
{
public:
    explicit NetworkError(std::string message)
        : ServerError(std::move(message)) {}
};

/// thrown for file/db open/read/write issues.
class DataBaseError final : public ServerError
{
public:
    explicit DataBaseError(std::string message)
        : ServerError(std::move(message))  {}
};

/// thrown when user registry/state invariants are violated.
class RegistryError final : public ServerError 
{
public:
    explicit RegistryError(std::string message)
        : ServerError(std::move(message)) {}
};

/// thrown for thread/lifecycle problems.
class LifecycleError final : public ServerError 
{
public:
    explicit LifecycleError(std::string message)
        : ServerError(std::move(message)) {}
};

}  