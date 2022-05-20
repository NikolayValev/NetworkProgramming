#include <iostream>
#include "DisplayChat.h"
#include "../Client.h"
#include <Winsock2.h>
#include <vector>
#include <iostream>
#include <string>
#include <process.h>
#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

using namespace std;

DisplayChat::DisplayChat()
{
    cout << "Starting display" << endl;
    initialisationFunction();
    ConnectToClient();
    while (true)
    {
        Message myMessage = ReceiveMsg(m_clientConn);
        DisplayMessage(myMessage);
    }

}
/*
NAME
        DisplayChat::initialisationFunction -Starts the connection with client
SYNOPSIS
        DisplayChat::initialisationFunction()
DESCRIPTION
        connects to client

*/
void DisplayChat::initialisationFunction() {
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(1, 1);

    err = WSAStartup(wVersionRequested, &wsaData);

    if (err != 0) {

        cerr << "Could not find useable DLL" << endl;
        exit(EXIT_FAILURE);
    }

    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
    {
        WSACleanup();
        cerr << "Could not find useable DLL" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Allocating listening socket" << endl;
    struct servent* pse;
    struct sockaddr_in sin;

    memset((char*)&sin, 0, sizeof(sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    if ((pse = getservbyname(m_clientService.c_str(), "tcp")) != NULL)
    {
        sin.sin_port = (u_short)pse->s_port;
    }
    else if ((sin.sin_port = htons((u_short)stoi(m_clientService))) == 0)
    {
        cerr << "Bad Port number/service specified: " << m_clientService << endl;;
        exit(1);
    }
    if ((m_clientConn = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        int errorcode = WSAGetLastError();
        cerr << "socket call failed: " << errorcode << endl;
        exit(1);
    }
    if (bind(m_clientConn, (struct sockaddr*)&sin, sizeof(sin)) == INVALID_SOCKET)
    {
        int errorcode = WSAGetLastError();
        cerr << "bind call failed: " << errorcode << endl;
        exit(1);
    }
    if (listen(m_clientConn, 5) == INVALID_SOCKET)
    {
        int errorcode = WSAGetLastError();
        cerr << "listen call failed: " << errorcode << endl;
        exit(1);
    }
    cout << "Listening socket: " << m_clientConn << endl;

    struct sockaddr_in fsin; 
    int alen = sizeof(sockaddr_in);

    m_clientConn = accept(m_clientConn, (struct sockaddr*)&fsin, &alen);
    if (m_clientConn == INVALID_SOCKET) {
        int errorcode = WSAGetLastError();
        cout << "Error accepting a client " << errorcode << endl;
    }
    cout << "Connected to client" << endl;
}

/*
NAME
        DisplayChat::DisplayMessage -Shows the messages
SYNOPSIS
        DisplayChat::DisplayMessage(Client::Message )
DESCRIPTION
        Calls the destructor and ends the server

*/
void DisplayChat::DisplayMessage(Client::Message userInput)
{
    cout << userInput.username << ": " << userInput.content << endl;
}



/*
NAME
        DisplayChat::~DisplayChat -Deletes the DisplayChat function
SYNOPSIS
        DisplayChat::~DisplayChat()
DESCRIPTION
        Calls the destructor and ends the server

*/
DisplayChat::~DisplayChat()
{
    cout << "Guten Nacht!" << endl;
    Sleep(5000);
    cout << "Wait. Did I have to do anything else? No? Okay then." << endl;
}