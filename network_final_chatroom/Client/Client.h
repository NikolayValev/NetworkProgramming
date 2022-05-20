#pragma once
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#include <Winsock2.h>
#include <vector>
#include <iostream>
#include <string>
#include <process.h>
#define MESSAGE_HEADER 24
#define MESSAGE_LENGTH 300
#define TOTAL_LENGTH 324
class Client {
public:
	struct Message {
		string username;
		int messageKind;
		int length;
		string content;
		char message[TOTAL_LENGTH];

		Message() 
		{
			username = "";
			messageKind = -8;
			length = 0;
			content = "";
			memset(message, 0, TOTAL_LENGTH);
		}
	};
	Client();
	~Client();
	Message ReceiveMsg(SOCKET userSocket);
	string m_username;
	string m_password;
	string m_host;
	const string m_chatService = "8081";
	const string m_displayService = "8082";
	void ClientToServer();
	void ServerToDisplay();
private:
	SOCKET m_messagingSocket;
	SOCKET m_displayingSocket;
	SOCKET connectToServer(string p_host, string p_service);
	void ReceiveCredentials();
	bool AreYehWinningSon(string p_username, string p_password);
	void MessageSending(SOCKET userSocket, Message* userMessage);
	void ConvertiumMessagum(Message* userMessage);
	void LaunchDisplay();
};