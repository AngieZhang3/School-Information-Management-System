#include "CMySqlTask.h"
#include "Proto.h"
#include <fstream>
#include <iostream>
#include <random>
#include <string>
void CMySqlTask::DoTask()
{
	printf("Start DoTask ...");
	ConnectMySQL();
	MYSQL_RES* res;
	MYSQL_ROW row;
	//u_long nBlock = 1;
	//if (ioctlsocket(m_clientSocket, FIONBIO, &nBlock) < 0) {
	//	LOGE("ioctlsocket");
	//	return;
	//}
		// handle errors
	char* pBuf = nullptr;
	while (true)
	{
		stPacketHdr hdr;
		if (recv(m_clientSocket, (char*)&hdr, sizeof(hdr), 0) < 0)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAECONNRESET || errorCode == WSAECONNABORTED)
			{
				LOGE("Client disconnected");
				break; //Safely exit the loop, handling connection closure
			}
			else {
				LOGE("recv");
				break;
			}

		}
		switch (hdr.nCmd) {
		case MYSQL_LOGIN:

			if (hdr.nLen > 0)
			{
				pBuf = new char[hdr.nLen + 1];
				if (recv(m_clientSocket, pBuf, hdr.nLen, 0) < 0)
				{
					LOGE("recv");
					delete[] pBuf;
					continue;
				}
				pBuf[hdr.nLen] = '\0';

				if (mysql_query(&m_mysql, pBuf) < 0)
				{
					LOGE("myslq_query");
					printf("Login query failed！");
					mysql_close(&m_mysql);
					delete[] pBuf;
					continue;
				}
				res = mysql_store_result(&m_mysql);
				if (res == NULL)
				{
					std::string errorMessage = mysql_error(&m_mysql);
					LOGE_A("mysql_store_result", errorMessage.c_str());
				}
				//Get result
				if ((row = mysql_fetch_row(res)) != NULL) {
					if (atoi(row[0]) > 0) {
						// user name and password are verified successfully

						if (send(m_clientSocket, "LOGIN_OK", strlen("LOGIN_OK"), 0) == SOCKET_ERROR)
						{
							LOGE("send");
						}

					}
					else if (send(m_clientSocket, "LOGIN_ERR", strlen("LOGIN_ERR"), 0) == SOCKET_ERROR)
					{
						LOGE("send");
					}
				}
			}
			delete[] pBuf;
			break;
		case STU_ADD:
		case CLASS_ADD:
		case COURSE_ADD:
		case RECORD_ADD:
			if (hdr.nLen > 0)
			{
				pBuf = new char[hdr.nLen + 1];
				if (recv(m_clientSocket, pBuf, hdr.nLen, 0) < 0)
				{
					LOGE("recv");
					delete[] pBuf;
					continue;
				}
				pBuf[hdr.nLen] = '\0';

				{
					std::lock_guard<std::mutex> lock(db_mutex);
					if (mysql_real_query(&m_mysql, pBuf, hdr.nLen) != 0)
					{
						stPacketHdr sentHdr;
						sentHdr.nCmd = hdr.nCmd + 2;
						sentHdr.nLen = 0;
						if (send(m_clientSocket, (char*)&sentHdr, sizeof(sentHdr), 0) == SOCKET_ERROR)
						{
							LOGE("send");
						}
						printf("ADD_ERR has been sent\r\n");
					}

					else {
						stPacketHdr sentHdr;
						sentHdr.nCmd = hdr.nCmd + 1;
						sentHdr.nLen = 0;
						if (send(m_clientSocket, (char*)&sentHdr, sizeof(sentHdr), 0) == SOCKET_ERROR)
						{
							LOGE("send");
						}
						printf("ADD_OK has been sent\r\n");
					}
				}
			}
			delete[] pBuf;
			break;
		case STU_DEL:
		case CLASS_DEL:
		case COURSE_DEL:
		case RECORD_DEL:
			if (hdr.nLen > 0)
			{
				pBuf = new char[hdr.nLen + 1];
				if (recv(m_clientSocket, pBuf, hdr.nLen, 0) < 0)
				{
					LOGE("recv");
					delete[] pBuf;
					continue;
				}
				pBuf[hdr.nLen] = '\0';
				{
					std::lock_guard<std::mutex> lock(db_mutex);
					if (mysql_real_query(&m_mysql, pBuf, hdr.nLen) != 0)
					{
						stPacketHdr sentHdr;
						sentHdr.nCmd = hdr.nCmd + 2;
						sentHdr.nLen = 0;
						if (send(m_clientSocket, (char*)&sentHdr, sizeof(sentHdr), 0) == SOCKET_ERROR)
						{
							LOGE("send");
						}
						printf("DEL_ERR has been sent\r\n");
					}

					else {
						stPacketHdr sentHdr;
						sentHdr.nCmd = hdr.nCmd + 1;
						sentHdr.nLen = 0;
						if (send(m_clientSocket, (char*)&sentHdr, sizeof(sentHdr), 0) == SOCKET_ERROR)
						{
							LOGE("send");
						}
						printf("DEL_OK has been sent\r\n");
					}
				}
			}
			delete[] pBuf;
			break;
		case STU_QUERY:
		case CLASS_QUERY:
		case COURSE_QUERY:
		case RECORD_QUERY:
			if (hdr.nLen > 0)
			{
				pBuf = new char[hdr.nLen + 1];
				if (recv(m_clientSocket, pBuf, hdr.nLen, 0) < 0)
				{
					LOGE("recv");
					delete[] pBuf;
					continue;
				}
				pBuf[hdr.nLen] = '\0';
				if (mysql_real_query(&m_mysql, pBuf, hdr.nLen) != 0)
				{
					stPacketHdr sentHdr;
					sentHdr.nCmd = hdr.nCmd + 2;
					sentHdr.nLen = 0;
					if (send(m_clientSocket, (char*)&sentHdr, sizeof(sentHdr), 0) == SOCKET_ERROR)
					{
						LOGE("send");
					}
					printf("QUERY_ERR has been sent\r\n");
				}

				else {
					MYSQL_RES* result = mysql_store_result(&m_mysql);
					if (result == NULL)
					{
						stPacketHdr sentHdr;
						sentHdr.nCmd = hdr.nCmd + 3;
						sentHdr.nLen = 0;
						if (send(m_clientSocket, (char*)&sentHdr, sizeof(sentHdr), 0) == SOCKET_ERROR)
						{
							LOGE("send");
						}
						printf("QUERY_NULL has been sent\r\n");
					}
					else {
						// Creating a random number generator
						std::random_device rd;  //Random number seed
						std::mt19937 gen(rd()); // Random number generator seeded with rd
						std::uniform_int_distribution<> distrib(1, 100); // Define a distribution with a range of 1-10

						// Generate a random number
						int randomNumber = distrib(gen);

						// Construct a new file name, adding a random number
						std::string newFilename = "result" + std::to_string(randomNumber) + ".txt";
						std::ofstream file(newFilename);
						if (file.is_open())
						{
							MYSQL_ROW row;
							while ((row = mysql_fetch_row(result)))
							{
								int numFields = mysql_num_fields(result);
								for (int i = 0; i < numFields; i++)
								{
									file << row[i] << '\t';
								}
								file << '\n';
							}
							file.close();

							sendFileToClient(m_clientSocket, newFilename, hdr.nCmd);

							// Free the result set when you're done with it

							/*const char* endOfFileMsg = "END_OF_FILE";
							if (send(m_clientSocket, endOfFileMsg, strlen(endOfFileMsg), 0) == SOCKET_ERROR)
							{
								LOGE("send");
							}*/
						}
						else {
							LOGE("is_open");
							break;
						}

					}
					mysql_free_result(result);
				}

			}
			delete[] pBuf;
			break;
		case STU_EDIT_CHECK:
		case CLASS_EDIT_CHECK:
		case COURSE_EDIT_CHECK:
		case RECORD_EDIT_CHECK:
			if (hdr.nLen > 0)
			{
				pBuf = new char[hdr.nLen + 1];
				if (recv(m_clientSocket, pBuf, hdr.nLen, 0) < 0)
				{
					LOGE("recv");
					delete[] pBuf;
					continue;
				}
				pBuf[hdr.nLen] = '\0';
				if (mysql_real_query(&m_mysql, pBuf, hdr.nLen) != 0)
				{
					stPacketHdr sentHdr;
					sentHdr.nCmd = hdr.nCmd + 3;
					sentHdr.nLen = 0;
					if (send(m_clientSocket, (char*)&sentHdr, sizeof(sentHdr), 0) == SOCKET_ERROR)
					{
						LOGE("send");
					}
					printf("EDIT_ERR has been sent\r\n");
				}

				else {
					res = mysql_store_result(&m_mysql);
					if (res == NULL)
					{
						std::string errorMessage = mysql_error(&m_mysql);
						LOGE_A("mysql_store_result", errorMessage.c_str());
					}
					// get result
					if ((row = mysql_fetch_row(res)) != NULL) {
						if (atoi(row[0]) <= 0) {
							//id does not exist
							stPacketHdr sentHdr;
							sentHdr.nCmd = hdr.nCmd + 1;
							sentHdr.nLen = 0;
							if (send(m_clientSocket, (char*)&sentHdr, sizeof(sentHdr), 0) == SOCKET_ERROR)
							{
								LOGE("send");
							}
						}
					}
				}
			}
			delete[] pBuf;
			break;
		case STU_EDIT:
		case CLASS_EDIT:
		case COURSE_EDIT:
		case RECORD_EDIT:
			if (hdr.nLen > 0)
			{
				pBuf = new char[hdr.nLen + 1];
				if (recv(m_clientSocket, pBuf, hdr.nLen, 0) < 0)
				{
					LOGE("recv");
					delete[] pBuf;
					continue;
				}
				pBuf[hdr.nLen] = '\0';
				{
					std::lock_guard<std::mutex> lock(db_mutex); // add a lock 
					if (mysql_real_query(&m_mysql, pBuf, hdr.nLen) != 0)
					{
						stPacketHdr sentHdr;
						sentHdr.nCmd = hdr.nCmd + 4;
						sentHdr.nLen = 0;
						if (send(m_clientSocket, (char*)&sentHdr, sizeof(sentHdr), 0) == SOCKET_ERROR)
						{
							LOGE("send");
						}
						printf("EDIT_ERR has been sent\r\n");
					}

					else {
						stPacketHdr sentHdr;
						sentHdr.nCmd = hdr.nCmd + 3;
						sentHdr.nLen = 0;
						if (send(m_clientSocket, (char*)&sentHdr, sizeof(sentHdr), 0) == SOCKET_ERROR)
						{
							LOGE("send");
						}
						printf("EDIT_OK has been sent\r\n");
					}
				}
				delete[] pBuf;
			}
			break;
		default:
			LOGE("Unknown command");
			break;

		}
	}

	while ((res = mysql_store_result(&m_mysql)) != NULL)
	{
		mysql_free_result(res);
		if (mysql_next_result(&m_mysql) > 0) {
			break;
		}
	}
	mysql_close(&m_mysql);
	OutputDebugString("DoTask ok");
}

void CMySqlTask::ConnectMySQL()
{
	if (mysql_library_init(0, NULL, NULL)) {
		LOGE("mysql_library_init");
		exit(1);
	}

	mysql_init(&m_mysql);

	// Set character set
	mysql_options(&m_mysql, MYSQL_SET_CHARSET_NAME, "gbk");

	// Read configuration file
	std::string configFile = "./config.ini";
	std::string dbHost = GetIniValue("Database", "Host", configFile);
	std::string dbUser = GetIniValue("Database", "User", configFile);
	std::string dbPwd = GetIniValue("Database", "Password", configFile);
	std::string dbName = GetIniValue("Database", "DBName", configFile);
	std::string strPort = GetIniValue("Database", "Port", configFile);
	int dbPort = atoi(strPort.c_str());

	mysql_options(&m_mysql, MYSQL_SET_CHARSET_NAME, "gbk");

	//连接数据
	if (!mysql_real_connect(&m_mysql,
		dbHost.c_str(),
		dbUser.c_str(),
		dbPwd.c_str(),
		dbName.c_str(),   //If there is no existing database, write NULL and add use when using it.
		dbPort,    //non real has no port
		NULL, 0))
	{
		std::string errorMessage = mysql_error(&m_mysql);
		LOGE_A("mysql_real_connect", errorMessage.c_str());
		return;
	}


	//// Try to connect to the MySQL server without specifying a database
	//if (!mysql_real_connect(&m_mysql, dbHost.c_str(), dbUser.c_str(), dbPwd.c_str(), NULL, 0, NULL, 0)) {
	//	std::string errorMessage = mysql_error(&m_mysql);
	//	LOGE_A("mysql_real_connect", errorMessage.c_str());
	//	exit(1);
	//}
	//else {
	//	// Check if the database exists
	//	if (mysql_select_db(&m_mysql, dbName.c_str()) != 0) {
	//		// If the database does not exist, create it
	//		std::string createQuery = "CREATE DATABASE " + dbName;
	//		if (mysql_query(&m_mysql, createQuery.c_str())) {
	//			std::string errorMessage = mysql_error(&m_mysql);
	//			LOGE_A("Create Database", errorMessage.c_str());
	//			exit(1);
	//		}

	//		// Now connect to the newly created database
	//		if (mysql_select_db(&m_mysql, dbName.c_str()) != 0) {
	//			std::string errorMessage = mysql_error(&m_mysql);
	//			LOGE_A("mysql_select_db", errorMessage.c_str());
	//			exit(1);
	//		}
	//	}

		// Database connection successful
		printf("Connected to the database successfully\r\n");

}

std::string CMySqlTask::GetIniValue(const std::string& section, const std::string& key, const std::string& filePath)
{
	char buffer[1024];
	GetPrivateProfileStringA(section.c_str(), key.c_str(), "", buffer, sizeof(buffer), filePath.c_str());
	return std::string(buffer);
}

void CMySqlTask::sendFileToClient(int clientSocket, const std::string& filename, int inputCmd) {
	// Open the file for reading
	std::ifstream file(filename, std::ios::binary);

	if (!file) {
		std::cerr << "Error: Unable to open file for sending." << std::endl;
		return;
	}

	// Get the file size
	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// Create and send the header
	stPacketHdr header;
	header.nCmd = inputCmd + 1; // Set your command value here
	header.nLen = htonl(fileSize);

	if (send(clientSocket, (char*)&header, sizeof(header), 0) == -1) {
		std::cerr << "Error: Failed to send header." << std::endl;
		file.close();
		return;
	}

	// Send the file data in chunks
	const int bufferSize = 1024;
	char buffer[bufferSize];
	size_t bytesRead;

	while ((bytesRead = file.read(buffer, sizeof(buffer)).gcount()) > 0) {
		if (send(clientSocket, buffer, bytesRead, 0) == -1) {
			std::cerr << "Error: Failed to send file data." << std::endl;
			file.close();
			return;
		}
	}

	// Close the file
	file.close();
	printf("file sent\r\n");
}
