#pragma once

namespace messaging
{

    // REQUESTS CONSTANTS:
    // field sizes in bytes
    constexpr size_t REQUEST_DATA_LENGTH_SIZE = 4; // size of data length field 
    constexpr size_t REQUEST_TYPE_SIZE = 2;        // size of request type field 
    constexpr size_t PROTOCOL_VERSION = 1;            // version field size 
    constexpr size_t REQUEST_HEADER_SIZE = 7;     // total  request header size 

    // request offsets
    constexpr size_t REQUEST_DATA_LENGTH_OFFSET = 0; // bytes 0-3: Data length field
    constexpr size_t REQUEST_TYPE_OFFSET = 4;        // bytes 4-6: Request type field  
    constexpr size_t PROTOCOL_VERSION_OFFSET = 6;    // byte 7: protocol version field

    // request data seperator
    constexpr char REQUEST_DATA_SEPERATOR = ':';

    // RESPONSE CONSTANTS:
    constexpr size_t RESPONSE_DATA_LENGTH_SIZE = 4;     // size of data len 
    constexpr size_t RESPONSE_CODE_SIZE = 2;           // size header
    constexpr size_t RESPONSE_HEADER_SIZE = 6;           // size header

    // response offsets
    constexpr size_t RESPONSE_DATA_LENGTH_OFFSET = 0; // bytes 0-3: Data length field
    constexpr size_t RESPONSE_CODE_OFFSET = 4;       // bytes 4: Data length field

    // MAXIMUM VALUES
    constexpr size_t MAX_MESSAGE_LENGTH = 9999;    // maximum message length
    constexpr size_t MAX_CLIENT_BYTES = 9999;     // maximum client buffer size


    // request action types
    enum class RequestType : int
    {
        INVALID = -1,
        GET_CHAT = 1,
        SEND_MESSAGE = 2,
        REGISTER = 3,
        DIRECT_MESSAGE = 4
    };

    enum class ResponseCode : int
    {
        NO_RESPONSE = -1,
        OK = 0,
        ABORTED_ERR = 1,
        NOT_REGISTER_ERR = 2,
        USER_NOT_FOUND_ERR = 3,
        DATABASE_ERR = 4,
        PROTOCOL_ERR = 5,
        REGISTRY_ERR = 6
    };
}
