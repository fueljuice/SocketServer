#pragma once

namespace messaging
{

    // REQUESTS CONSTANTS:
	// field sizes in bytes
    constexpr size_t REQUEST_DATA_LENGTH_SIZE = 4; // size of data length field 
    constexpr size_t REQUEST_TYPE_SIZE = 4;        // size of request type field 
    constexpr size_t PROTOCOL_VERSION = 1;            // version field size 
    constexpr size_t REQUEST_HEADER_SIZE = 9;     // total  request header size 
    
    // request offsets
    constexpr size_t REQUEST_DATA_LENGTH_OFFSET = 0; // bytes 0-3: Data length field
    constexpr size_t REQUEST_TYPE_OFFSET = 4;        // bytes 4-7: Request type field  
    constexpr size_t PROTOCOL_VERSION_OFFSET = 8;    // byte 8: protocol version field

	// request data seperator
	constexpr char REQUEST_DATA_SEPERATOR = ':';

	// RESPONSE CONSTANTS:
    constexpr size_t RESPONSE_DATA_LENGTH_SIZE = 4;      // size of data length
    constexpr size_t RESPONSE_HEADER_SIZE = 4;           // size header

    // MAXIMUM VALUES
    constexpr size_t MAX_MESSAGE_LENGTH = 9999;    // maximum message length
    constexpr size_t MAX_CLIENT_BYTES = 9999;     // maximum client buffer size

    
    // request action types
    enum class ActionType : int
    {
        INVALID = -1,
        GET_CHAT = 1,
        SEND_MESSAGE = 2,
        REGISTER = 3,
        DIRECT_MESSAGE = 4
    };
}
