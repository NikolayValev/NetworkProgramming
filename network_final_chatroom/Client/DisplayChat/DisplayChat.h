#ifndef CHAT_ROOM_DISPLAYCHAT_H
#define CHAT_ROOM_DISPLAYCHAT_H

#include "..\Client.h"
#include <Winsock2.h>
#include <vector>
#include <iostream>
#include <string>
#include <process.h>
#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
class DisplayChat
{
public:
    DisplayChat();
    void initialisationFunction();
    void DisplayMessage(Message userInput);
    ~DisplayChat();
private:
    string m_localHost = "127.0.0.1";
    string m_clientService = "8082";
    SOCKET m_clientConn;
};

#endif