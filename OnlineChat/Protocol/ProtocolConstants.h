#pragma once

namespace messaging
{

    // REQUESTS CONSTANTS:
	// field sizes in bytes
    constexpr int REQUEST_DATA_LENGTH_SIZE = 4; // size of data length field 
    constexpr int REQUEST_TYPE_SIZE = 4;        // size of request type field 
    constexpr int PROTOCOL_VERSION = 1;            // version field size 
    constexpr int REQUEST_HEADER_SIZE = 9;     // total  request header size 
    
    // request offsets
    constexpr int REQUEST_DATA_LENGTH_OFFSET = 0; // bytes 0-3: Data length field
    constexpr int REQUEST_TYPE_OFFSET = 4;        // bytes 4-7: Request type field  
    constexpr int PROTOCOL_VERSION_OFFSET = 8;    // byte 8: protocol version field

	// request data seperator
	constexpr char REQUEST_DATA_SEPERATOR = ':';

	// RESPONSE CONSTANTS:
    constexpr int RESPONSE_DATA_LENGTH_SIZE = 4;      // size of data length
    constexpr int RESPONSE_HEADER_SIZE = 4;           // size header

    // MAXIMUM VALUES
    constexpr int MAX_MESSAGE_LENGTH = 9999;    // maximum message length
    constexpr int MAX_CLIENT_BYTES = 9999;     // maximum client buffer size

    
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
