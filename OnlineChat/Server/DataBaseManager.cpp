#include "DataBaseManager.h"
#define dbFileDir   "C:\\Users\\zohar\\Desktop\\dbFile.txt"


void sockets::server::DataBaseManager::dbInit()
{
	std::lock_guard<std::mutex> lk(fileMutex);
	if (!dbFile.is_open())
		dbFile.open(dbFileDir, std::ios::in | std::ios::out | std::ios::app);
}

void sockets::server::DataBaseManager::dbClose()
{
	std::lock_guard<std::mutex> lk(fileMutex);
	if (dbFile.is_open())
		dbFile.close();
}

void sockets::server::DataBaseManager::writeToDB(std::string_view payload)
{
	// reading from database
	{
		std::lock_guard<std::mutex> lk(fileMutex);
		// check for errors in fie
		if (!dbFile.is_open())
			throw DataBaseError("unavaiable database");

		// clears previos handle
		dbFile.clear();
		dbFile.seekp(0, std::ios::end);

		// writes to database the message
		dbFile << payload << std::endl;
		dbFile.flush();
	}
}

std::string sockets::server::DataBaseManager::readDB() const
{
	std::lock_guard<std::mutex> lk(fileMutex);
	if (!dbFile.is_open())
		throw DataBaseError("unavaiable database");


	// clears previous handle.
	dbFile.clear();
	dbFile.seekg(0, std::ios::beg);


	// reads entire file into string
	std::string allText = std::string
	{
		std::istreambuf_iterator<char>(dbFile),
		std::istreambuf_iterator<char>()
	};


	// checks if the message is too big
	if (allText.size() > messaging::MAX_MESSAGE_LENGTH)
		allText.resize(messaging::MAX_MESSAGE_LENGTH);

	// check if reads was 
	if (!dbFile.good() && !dbFile.eof())
		throw DataBaseError("could not fetch data from the database");

	return allText;
}
