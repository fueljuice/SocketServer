#include "ChatRequestHandler.h"

#include "../Protocol/ProtocolConstants.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

void sockets::server::ChatRequestHandler::handleGetChat(
	SOCKET clientSocket,
	messaging::ParsedRequest& pr,
	std::fstream& dbFile,
	std::mutex& fileMutex,
	const std::function<bool(SOCKET, const char*, int)>& sendAll)
{
	DBG("getChat request called");
	std::string allText;
	{
		// locking file to prevent race writing
		std::lock_guard<std::mutex> lk(fileMutex);
		if (!dbFile.is_open())
			return;


		// clears previous handle.
		dbFile.clear();
		dbFile.seekg(0, std::ios::beg);


		// reads entire file into string
		allText = std::string
		{
			std::istreambuf_iterator<char>(dbFile),
			std::istreambuf_iterator<char>()
		};


		// checks if the message is too big
		if (allText.size() > messaging::MAX_MESSAGE_LENGTH)
			allText.resize(messaging::MAX_MESSAGE_LENGTH);

		// check if reads was 
		if (!dbFile.good() && !dbFile.eof())
			return;
	}
	// sends using the lambda function defined in the server class
	DBG("allText:" << allText << "\nsize of allText: " << allText.size());
	sendAll(clientSocket, allText.c_str(), static_cast<int>(allText.size()));

}

void sockets::server::ChatRequestHandler::handleSendMessage(
	SOCKET clientSocket,
	messaging::ParsedRequest& pr,
	std::fstream& dbFile,
	std::mutex& fileMutex,
	std::mutex& nameMapMutex,
	std::unordered_map<SOCKET, std::string>& clientsNameMap,
	const std::function<void(const char*, int)>& broadcast,
	const std::function<bool(SOCKET, const char*, int)>& sendAll)
{
	// cant send message before registering
	{
		std::lock_guard<std::mutex> lk(nameMapMutex);
		if (clientsNameMap[clientSocket].empty())
		{
			const char* msg = "please register.";
			sendAll(clientSocket, msg, static_cast<int>(strlen(msg)));
			return;
		}
	}

	DBG("sendMessage request called");
	// reading from database
	{
		std::lock_guard<std::mutex> lk(fileMutex);
		// check for errors in fie
		if (!dbFile.is_open())
			return;
		
		// clears previos handle
		dbFile.clear();
		dbFile.seekp(0, std::ios::end);
		// writes to database the message
		{
			std::lock_guard<std::mutex> lkName(nameMapMutex);
			dbFile << clientsNameMap[clientSocket] << ": " << pr.dataBuffer << std::endl;
		}
		dbFile.flush();
	}
	// brodcasts the message
	broadcast(pr.dataBuffer.c_str(), static_cast<int>(pr.dataBuffer.length()));
}

bool sockets::server::ChatRequestHandler::handleRegister(
	SOCKET clientSocket,
	messaging::ParsedRequest& pr,
	std::mutex& nameMapMutex,
	std::unordered_map<SOCKET, std::string>& clientsNameMap,
	const std::function<bool(SOCKET, const char*, int)>& sendAll)
{


	// checks if client is already registered
	{
		std::lock_guard<std::mutex> lk(nameMapMutex);
		if (!clientsNameMap[clientSocket].empty())
		{
			const char* msg = "you are already registered.";
			sendAll(clientSocket, msg, static_cast<int>(strlen(msg)));
			return false;
		}
	}

	DBG("start regitering");
	{
		std::lock_guard<std::mutex> lk(nameMapMutex);
		// checks if username is already taken
		for (const auto& pair : clientsNameMap)
		{
			if (pair.second == pr.dataBuffer)
			{
				// if taken, notify the user
				const char* msg = "this username was already taken......";
				sendAll(clientSocket, msg, static_cast<int>(strlen(msg)));
				return false;
			}
		}
		//if all OK. map client to username
		clientsNameMap[clientSocket] = pr.dataBuffer;
	}
	// sends sucsess message
	const char* successMsg = "registration successful.";
	sendAll(clientSocket, successMsg, static_cast<int>(strlen(successMsg)));
	return true;
}
void sockets::server::ChatRequestHandler::handleDirectMessage(
	SOCKET clientSocket,
	messaging::ParsedRequest& pr,
	std::mutex& nameMapMutex,
	std::unordered_map<SOCKET, std::string>& clientsNameMap,
	const std::function<bool(SOCKET, const char*, int)>& sendAll)
{
	std::string senderUsername;
	{
		// checks if user didnt register
		std::lock_guard<std::mutex> lk(nameMapMutex);
		senderUsername = clientsNameMap[clientSocket];
		if (senderUsername.empty())
		{
			const char* msg = "please register.";
			sendAll(clientSocket, msg, static_cast<int>(strlen(msg)));
			return;
		}
	}
	
	std::lock_guard<std::mutex> lk(nameMapMutex);
	// seperate recver name from message
	std::size_t seperatorPos = pr.dataBuffer.find(messaging::REQUEST_DATA_SEPERATOR);
	if (seperatorPos == std::string::npos)
	{
		// enforce seperations
		std::string errMsg = "DM usage: {recver}" + std::to_string(messaging::REQUEST_DATA_SEPERATOR) + "{message}";
		sendAll(clientSocket, errMsg.c_str(), static_cast<int>(strlen(errMsg.c_str())));
		return;
	}
	const std::string targetUserName = pr.dataBuffer.substr(0, seperatorPos);
	const std::string messageContent = pr.dataBuffer.substr(seperatorPos + 1);

	for (const auto& [curSocket, curUserName] : clientsNameMap)
	{
		// sends the request user a message.
		DBG("looking for user:" << targetUserName << " in name map. current pair: " << curUserName);
		DBG("is equal: " << (curUserName == targetUserName));
		if (curUserName == targetUserName)
		{
			const std::string msg = "(DM from " + senderUsername + "): " + messageContent;
			sendAll(curSocket, msg.c_str(), msg.size());
			return;
		}
	}
	// if user not found. return err to sender
	const char* errMsg = "user not found.";
	sendAll(clientSocket, errMsg, static_cast<int>(strlen(errMsg)));
}
