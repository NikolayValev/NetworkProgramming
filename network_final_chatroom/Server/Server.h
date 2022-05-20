#include <iostream>
#include <map>
#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#include <Winsock2.h>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

class ServerClientNode {
public:
	int HEADER_LENGTH = 24;
	char headerData[24];      
	string userBuff;
	SOCKET clientSocket;       
	char message[300];  
	string messageBuff;
	int nbData;        
	int msgSize;
	int headerSizeRecv;  
	bool isVerified;

	ServerClientNode() 
	{
		msgSize = 0; 
		headerSizeRecv = 0;
		nbData = 0;
		memset(headerData, 0, 24);
		isVerified = false; 
	}

};

class Server{

public: 

	Server();
	~Server();
	int AcceptNewClient();
	void GoTalkToYourRelatives();
private:
	ifstream serverUserList;
	void Disconnect(int uniqueClient);
	map <string, string> user_pass;
	vector<ServerClientNode> allClientData;
	vector<SOCKET> m_all_sockets;
	SOCKET largestSocket;
	SOCKET serverListeningSoc;
	string m_service = "8080";
	fd_set readmap;
	bool AreYehWinningSon(int uniqueClient);
	bool ReceiveMsg(int uniqueClient);
	void messageSent(int uniqueClient);
	bool pendingConnections();
};
