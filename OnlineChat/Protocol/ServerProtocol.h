#pragma once


#include <iostream>
#include <stdlib.h>
#include <string>
#include <cstdio>
#include "parsedRequest.h"
#include <algorithm>

/**
 * @brief Handles server-side protocol parsing and response generation
 * 
 * This class provides static methods for parsing incoming client requests according to the
 * chat protocol format and generating appropriate responses. All methods are static and stateless.
 * 
 * Protocol format:
 * - Header (16 bytes):
 *   - Bytes 0-3: Data length (4-digit decimal, zero-padded)
 *   - Bytes 4-7: Request type (4-digit decimal, zero-padded)
 *   - Bytes 8-15: Username (8-byte fixed-width field, null-padded)
 * - Data: Variable length message content
 */
namespace messaging
{
class ServerProtocol
{
public:

	/**
	 * @brief Constructs a response header with data length
	 * 
	 * Creates a 4-byte response header containing the data length in decimal format,
	 * zero-padded to REQUEST_DATA_LENGTH_SIZE bytes.
	 * 
	 * @param length Length of the response data
	 * @return Formatted response header string
	 */
	static std::string constructResponseHeader(int length);
	
	/**
	 * @brief Parses request header and extracts metadata
	 * 
	 * Extracts data length, request type, and username from the 16-byte request header.
	 * Returns a ParsedRequest struct with the extracted information.
	 * 
	 * @param rawHeader Raw header data (must be REQUEST_HEADER_SIZE bytes)
	 * @param rawLength Length of the header data
	 * @return ParsedRequest with extracted header information
	 */
	static ParsedRequest parseHeader(const char* rawHeader, int rawLength);
	
	/**
	 * @brief Parses request data payload
	 * 
	 * Extracts the actual message data from the request payload and adds it to the
	 * ParsedRequest struct. Only called if request is valid and has data.
	 * 
	 * @param pr ParsedRequest containing header information (moved)
	 * @param rawData Raw data payload
	 * @return ParsedRequest with data populated
	 */
	static ParsedRequest parseData(ParsedRequest&& pr, char* rawData);
	
	/**
	 * @brief Validates if the complete request is valid
	 * 
	 * Checks if the request has all required fields (username, valid type) and
	 * appropriate data content for the request type.
	 * 
	 * @param pr ParsedRequest to validate
	 * @return true if request is valid, false otherwise
	 */
	static bool isStatusOK(const ParsedRequest& pr, bool isRegistered);
	
	/**
	 * @brief Checks if the header parsing was successful
	 * 
	 * Validates that essential header fields (username, data length, request type)
	 * were successfully extracted and are not invalid.
	 * 
	 * @param pr ParsedRequest containing header information
	 * @return true if header is valid, false otherwise
	 */
	static bool isHeaderOK(const ParsedRequest& pr);

private:
	/**
	 * @brief Extracts data length from header
	 * 
	 * Parses the first 0-3 bytes of the header as a decimal integer
	 * representing the length of the request data.
	 * 
	 * @param pr ParsedRequest to populate with length
	 * @param rawHeader Raw header data
	 */
	static void extractLength(ParsedRequest& pr, const char* rawHeader);
	
	/**
	 * @brief Extracts request type from header
	 * 
	 * Parses bytes 4-7 of the header as a decimal integer representing
	 * the request type (GET_CHAT, SEND_MESSAGE, REGISTER, DIRECT_MESSAGE).
	 * 
	 * @param pr ParsedRequest to populate with request type
	 * @param rawHeader Raw header data
	 */
	static void extractRequestType(ParsedRequest& pr, const char* rawHeader);
	
	/**
	 * @brief Extracts protocol version from header
	 * 
	 * Parses byte 8 of the header as a decimal integer representing
	 * the protocol version.
	 * 
	 * @param pr ParsedRequest to populate with protocol version
	 * @param rawHeader Raw header data
	 */
	static void extractProtocolVersion(ParsedRequest& pr, const char* rawHeader);
	
	
	/**
	 * @brief Extracts data payload from request
	 * 
	 * Copies the raw data payload into the ParsedRequest's dataBuffer.
	 * 
	 * @param pr ParsedRequest to populate with data
	 * @param rawData Raw data payload
	 */
	static void extractData(ParsedRequest& pr, const char* rawData);

};
}
