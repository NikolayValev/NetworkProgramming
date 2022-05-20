#include <iostream>
#include "Server.h"
#include <map>
#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#include <Winsock2.h>
#include <vector>
#include <string>
#include <fstream>
using namespace std;
/*
NAME
        Server::Server 
SYNOPSIS
        Server::Server();
DESCRIPTION
        This initialises the server object and sets the server up running, and even goes as far as to accept the first one!
AUTHOR
        Sanskar Raj Marahata
*/
Server::Server() {
	largestSocket = -2;


	//This part is basically passiveTCP from your code, professor
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);  
	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0) {
		cerr << "Could not find useable DLL" << endl;
		exit(1);
	}

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
		cerr << "Could not find useable DLL" << endl;
		WSACleanup();   
		exit(1);
	}
	
	struct servent* pse; 
	struct sockaddr_in sin; 

	memset((char*)&sin, 0, sizeof(sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;

	if ((pse = getservbyname(m_service.c_str(), "tcp")) != NULL)
	{
		sin.sin_port = (u_short)pse->s_port;
	}
	else if ((sin.sin_port = htons((u_short)stoi(m_service))) == 0)
	{
		cerr << "Bad Port number/service specified: " << m_service << endl;;
		exit(1);
	}
	if ((serverListeningSoc = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		int errorcode = WSAGetLastError();
		cerr << "socket call failed: " << errorcode << endl;
		exit(1);
	}
	if (bind(serverListeningSoc, (struct sockaddr*)&sin, sizeof(sin)) == INVALID_SOCKET)
	{
		int errorcode = WSAGetLastError();
		cerr << "bind call failed: " << errorcode << endl;
		exit(1);
	}
	if (listen(serverListeningSoc, 5) == INVALID_SOCKET)
	{
		int errorcode = WSAGetLastError();
		cerr << "listen call failed: " << errorcode << endl;
		exit(1);
	}
	cout << "Listening socket: " << serverListeningSoc << endl;
	//We add and test
	m_all_sockets.push_back(serverListeningSoc);
	largestSocket = serverListeningSoc;
	serverUserList.open("authenticationdocs.txt");
	if (serverUserList.is_open()) {
		string file_username;
		string file_password;

		while (serverUserList >> file_username >> file_password) {
			user_pass.insert({ file_username, file_password });
		}
	}
}
/*
NAME
        Server::~Server 
SYNOPSIS
        Server::~Server();
DESCRIPTION
		This closes the individual node. It's great for memory cleanup!
AUTHOR
        Sanskar Raj Marahata
*/
Server::~Server() {
	WSACleanup();
}

/*
NAME
        Server::AcceptNewClient 
SYNOPSIS
        int Server::AcceptNewClient();
DESCRIPTION
		Accepts connection from a new client
RETURNS
		An integer variable with the client's id on it.
AUTHOR
        Sanskar Raj Marahata
*/
int Server::AcceptNewClient() {	
	//Check if new connections are waiting. If not, we can just go back
	if (!pendingConnections()) 
	{
		return -1;
	}
	if (FD_ISSET(serverListeningSoc, &readmap) == false) 
	{
		return 1;
	}
	SOCKET holdingSocket;
	struct sockaddr_in fsin;
	int alen = sizeof(sockaddr_in);  
	holdingSocket = accept(serverListeningSoc, (struct sockaddr*)&fsin, &alen);
	if (holdingSocket == INVALID_SOCKET) {
		int errorcode = WSAGetLastError();
		cout << "Error accepting a client " << errorcode << endl;
		return -1;
	}
	//Check to see which socket has the topmost priority
	if (holdingSocket > largestSocket)
	{
		largestSocket = holdingSocket;
	}
	//Resetting it for a fresh start every time
	for (int i = 0; i < (int)allClientData.size(); i++)
	{
		if (allClientData[i].clientSocket == 0)
		{
			m_all_sockets[i + 1] = holdingSocket;
			allClientData[i].clientSocket = holdingSocket;
			allClientData[i].msgSize = 0;
			allClientData[i].headerSizeRecv = 0;
			allClientData[i].nbData = 0;
			return i;   
		}
	}
	//Now that we've finally finalised everything, we can create a separate client node for
	// the client and add stuff in.
	ServerClientNode newClient;
	newClient.clientSocket = holdingSocket;
	m_all_sockets.push_back(holdingSocket);
	allClientData.push_back(newClient);
	return allClientData.size() - 1;
}
/*
NAME
        Server::GoTalkToYourRelatives()
SYNOPSIS
        void Server::AreYehWinningSon( );
DESCRIPTION
		Receives and sends messages

AUTHOR
        Sanskar Raj Marahata
*/
void Server::GoTalkToYourRelatives() {
	if (!pendingConnections()) {
		return;
	}
	for (unsigned int i = 0; i < allClientData.size(); i++) {
		if (!FD_ISSET(allClientData[i].clientSocket, &readmap)) { 
			continue;
		}
		ReceiveMsg(i);
		messageSent(i);
	}
}

/*
NAME
        Server::pendingConnections 
SYNOPSIS
        bool Server::pendingConnections();
DESCRIPTION
		Checks for new connections if they are pending in the read map
RETURNS
		An boolean that returns true when connections are pending and false when they aren't.
AUTHOR
        Sanskar Raj Marahata
*/
bool Server::pendingConnections() {
	FD_ZERO(&readmap);
	for (unsigned int i = 0; i < m_all_sockets.size(); i++)
	{
		if (m_all_sockets[i] == 0) 
		{
			continue;
		}
		FD_SET(m_all_sockets[i], &readmap);
		if (m_all_sockets[i] > largestSocket)
		{
			largestSocket = m_all_sockets[i];
		}
	}
	if (select(largestSocket + 1, &readmap, NULL, NULL, NULL) == SOCKET_ERROR) {
		WSACleanup();
		return false;
	}
	return true;
}

/*
NAME
        Server::ReceiveMsg 
SYNOPSIS
        bool Server::ReceiveMsg( int uniqueClient);
DESCRIPTION
		Receives the message from the client

		uniqueClient -> the unique number of the client in the memory of the server
RETURNS
		An boolean that returns true when successful and false when a failure.
AUTHOR
        Sanskar Raj Marahata
*/
bool Server::ReceiveMsg(int uniqueClient) {
	//create new instance for function
	ServerClientNode &cd = allClientData[uniqueClient];
	int nb;
	while (cd.headerSizeRecv < cd.HEADER_LENGTH) {
		nb = recv(cd.clientSocket, &cd.headerData[cd.headerSizeRecv], cd.HEADER_LENGTH - cd.headerSizeRecv, 0);
		if (nb <= 0)
		{
			closesocket(cd.clientSocket);
			Disconnect(uniqueClient);
			return false;
		}
		//receive it here
		cd.headerSizeRecv += nb;
		if (cd.headerSizeRecv == cd.HEADER_LENGTH)
		{
			cd.userBuff = cd.headerData;
			char msgLength[3];
			memmove(msgLength, &cd.headerData[21], 3);
			cd.msgSize = atoi(msgLength);
		}
	}
	//receive rest of data
	while (cd.nbData < cd.msgSize)
	{
		nb = recv(cd.clientSocket, &cd.message[cd.nbData], cd.msgSize - cd.nbData, 0);
		if (nb <= 0)
		{
			closesocket(cd.clientSocket);
			cd.clientSocket = 0;
			return false;
		}
		cd.nbData += nb;
	}
	cd.messageBuff = cd.message;
	cd.messageBuff = cd.messageBuff.substr(0, cd.msgSize);
	if (!cd.isVerified) {
		AreYehWinningSon(uniqueClient);
	}

	return true;
}

/*
NAME
        Server::AreYehWinningSon()
SYNOPSIS
        bool Server::AreYehWinningSon( int uniqueClient);
DESCRIPTION
		Checks if it is authenticated or not.

		uniqueClient -> the unique number of the client in the memory of the server
RETURNS
		An boolean that returns true when authenticated and false when not
AUTHOR
        Sanskar Raj Marahata
*/
bool Server::AreYehWinningSon(int uniqueClient) {
	ServerClientNode &cd = allClientData[uniqueClient];
	bool clientExists = false;
	cd.headerData[20] = '3'; 
	if (user_pass.find(cd.userBuff) != user_pass.end()) {
		if (user_pass[cd.userBuff] == cd.messageBuff) {
			clientExists = true;
			cd.isVerified = true;
			cd.headerData[20] = '2'; 
		}
	}

	return clientExists;
}

/*
NAME
        Server::messageSent
SYNOPSIS
        bool Server::messageSent( int uniqueClient);
DESCRIPTION
		Sends message to clients.

		uniqueClient -> the unique number of the client in the memory of the server

AUTHOR
        Sanskar Raj Marahata
*/
void Server::messageSent(int uniqueClient) {

	ServerClientNode &cd = allClientData[uniqueClient];

	if (cd.clientSocket == 0) {
		return;
	}
	//i'm basically sending it once, since send is atomic within the machine
	string meData = cd.headerData;
	meData += cd.message;
	if (cd.headerData[20] == '1') {
		for (unsigned int i = 0; i < allClientData.size(); ++i) {
			ServerClientNode &client = allClientData[i];
			
			send(client.clientSocket, meData.c_str(), meData.size(), 0);
		}
	}
	else {
		send(cd.clientSocket, meData.c_str(), meData.size(), 0);
	}

	memset(cd.message, 0, cd.msgSize);
	cd.messageBuff = "";
	cd.msgSize = 0;
	cd.nbData = 0;
	cd.headerSizeRecv = 0;
	memset(cd.headerData, '0', cd.HEADER_LENGTH);
	return;

}
/*
NAME
        Server::Disconnect()
SYNOPSIS
        int Server::Disconnect( int uniqueClient);
DESCRIPTION
		Disconnects the given client

		uniqueClient -> the unique number of the client in the memory of the server

AUTHOR
        Sanskar Raj Marahata
*/
void Server::Disconnect(int uniqueClient) {
	ServerClientNode &cd = allClientData[uniqueClient];
	FD_CLR(cd.clientSocket, &readmap);
	cd.clientSocket = 0;
	m_all_sockets[uniqueClient + 1] = 0;
	cd.~ServerClientNode();
}