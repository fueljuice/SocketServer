#pragma once

namespace messaging
{

    // REQUESTS CONSTANTS:
	// field sizes in bytes
    constexpr int REQUEST_DATA_LENGTH_SIZE = 4; // size of data length field 
    constexpr int REQUEST_TYPE_SIZE = 4;        // size of request type field 
    constexpr int USERNAME_SIZE = 8;            // username field size 
    constexpr int REQUEST_HEADER_SIZE = 16;     // total  request header size 

    constexpr int REQUEST_DATA_LENGTH_OFFSET = 0; // bytes 0-3: Data length field
    constexpr int REQUEST_TYPE_OFFSET = 4;        // bytes 4-7: Request type field  
    constexpr int USERNAME_OFFSET = 8;            // bytes 8-15: Username field

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
        REGISTER = 3
    };
}
