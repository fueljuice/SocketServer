#pragma once
#include <string>
#include <string_view>
#include <mutex>
#include <fstream>

#include "../Protocol/ProtocolConstants.h"
#include "../Server/ServerException.h"
namespace sockets::server
{
struct IdbManager
{
	virtual ~IdbManager() = default;
	virtual void dbInit() = 0;
	virtual void dbClose() = 0;
	virtual void writeToDB(std::string_view payload) = 0;
	virtual std::string readDB() const = 0;
};

class DataBaseManager : public IdbManager
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

