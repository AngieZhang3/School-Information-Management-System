#pragma once
#include "CThreadPool.h"
#include "log.h"
#include "include/mysql.h"
#pragma comment(lib, "libmysql")
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

class CMySqlTask : public ITask
{
public:
	CMySqlTask(int clientSocket)
	{
		m_clientSocket = clientSocket;
	}
	virtual void DoTask();
	void ConnectMySQL();
	std::string GetIniValue(const std::string& section, const std::string& key, const std::string& filePath);
private:
	int m_clientSocket;
	MYSQL m_mysql; // Declare a MySQL connection object
	std::mutex db_mutex; //Mutex lock for database operations
public: 
	void sendFileToClient(int clientSocket, const std::string& filename, int inputCmd);
};

