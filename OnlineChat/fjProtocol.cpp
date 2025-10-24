#include "FJProtocol.h"


FJProtocol::FJProtocol(const char* buf, int length)
	: 
	m_buffer(buf),
	bufLength(length) {}

bool FJProtocol::enforceProtocol()
{
	std::string uploadToFile;
	int length;
	char charlength[5];
	int whatAction;

	// according to the protocol, tries to read the first 4 bytes as the length
	if (!memcpy(charlength, m_buffer, 4))
		return false;
	charlength[4] = '\0';
	length = atoi(charlength);

	// if didnt read return false
	if(!length) 
		return false;
	

	whatAction = atoi(&m_buffer[5]);
	if (!whatAction)
		return false;
	
	//SENDMESSAGE
	if (!writeToFile())
		perror("COULDNT WRITE TO FILE");

	return true;
	

}

bool FJProtocol::writeToFile()
{
	std::fstream file(FILENAME, std::ios::out);
	for (int i = 5; i < bufLength; i++)
	{
		file << m_buffer[i];
	}

	if (!file.is_open())
		return false;
	

	return true;
}

