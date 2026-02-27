#pragma once
#include <string>
#include <string_view>
#include <mutex>
#include <fstream>

#include "../Protocol/ProtocolConstants.h"
#include "../Server/ServerException.h"
namespace sockets::server
{
class DataBaseManager
{

public:
	void dbInit();
	void dbClose();
	void writeToDB(std::string_view payload);
	std::string readDB() const;

private:
	mutable std::mutex fileMutex;
	mutable std::fstream dbFile;
};
}

