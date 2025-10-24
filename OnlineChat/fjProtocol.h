#pragma once
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>

#define  FILENAME	"chathistory.txt"

class FJProtocol
{

private:
	const char* m_buffer;
	int bufLength;
public:
	FJProtocol(const char* buf, int length);

//FJP PROTOCOL:
// FIRST FOUR BYTES: INTEGER LENGTH OF THE DATA
// BYTE NUMBER 5:
// ENUM ACTION:
// 1: GETCHAT
// 2: SENDMESSAGE
// IF GETCHAT: NO BYTES ARE READ
// IF SENDMESSAGE: CONSIDER BYTE 5 - 30,000 THE MESSAGE
// IT STORES IT IN A TXT FILE IN JSON FORMAT
//
//
	bool enforceProtocol();

	bool writeToFile();
};