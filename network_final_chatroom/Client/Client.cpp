#include "Client.h"
#include <Winsock2.h>
#include <vector>
#include <iostream>
#include <string>
#include <process.h>
#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

/*
NAME
        Client::Client -Starts the client
SYNOPSIS
        Client::Client();
DESCRIPTION
        Sets up the client, connects to server, and sets up the display

*/
Client::Client() {
    m_username = "";
    m_password = "";
    m_host = "";
    m_messagingSocket = NULL;
    m_displayingSocket = NULL;
    cout << "You thought it was the client? No! It's the client!" << endl;
    Sleep(1000);
    cout << " . . . . . . . . " << endl;
    Sleep(1000);
    cout << "Wait a minute. . . . " << endl;
    Sleep (1000);
    cout << "THIS IS THE CLIENT!" << endl;

    //It's basically your code for connectTCP, professor
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
    //Since we're doing it entirely locally, might as well do this?
    m_host = "127.0.0.1";

     
    m_messagingSocket = connectToServer(m_host, m_chatService);
    //Get email and password for authentication
    ReceiveCredentials();
    AreYehWinningSon(m_username, m_password);
    //Once authenticated, we launch the displaying process
    LaunchDisplay();
    m_displayingSocket = connectToServer(m_host, m_displayService);
}

/*
NAME
        Client::connectToServer - starts a connection to server
SYNOPSIS
        SOCKET Client::EstablishTCPConn(string p_host, string p_service);
        p_host -> local ip address
        p_service -> Intended connection(the service)
DESCRIPTION
        A socket is connected to the host
RETURNS
        A connected Socket

*/
SOCKET Client::connectToServer(string p_host, string p_service) {
    //Again, it's your code, professor.
    struct sockaddr_in sin;
    SOCKET s;

    memset((char*)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    if ((sin.sin_port = htons((short int)atoi(p_service.c_str())))== 0)
    {
        cerr << "Can't get the port number of the service." << endl;
        exit(EXIT_FAILURE);
    }

    if ((sin.sin_addr.s_addr = inet_addr(p_host.c_str())) == INADDR_NONE)
    {
        cerr << ("Invalid host address") << endl;
        exit(EXIT_FAILURE);
    }

    if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        cerr << "Could not allocate socket: " << WSAGetLastError() << endl;
        exit(EXIT_FAILURE);
    }

    if (connect(s, (struct sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        cerr << "Attempt to connect to server failed: " << p_service << endl;
        exit(EXIT_FAILURE);
    }

    return(s);
}

/*
NAME
        Client::AreYehWinningSon - Checks for authentication
SYNOPSIS
        bool Client::AreYehWinningSon(string p_username, string p_password);
        p_username -> The username provided by user
        p_password -> The password provided by user

DESCRIPTION
        We check for authentication in this program

*/
bool Client::AreYehWinningSon(string p_username, string p_password) {
    Message logCreds;
    logCreds.username = m_username;
    logCreds.messageKind = 0;
    logCreds.length = m_password.size();
    logCreds.content = m_password;
    //Sending messages
    MessageSending(m_messagingSocket, &logCreds);

    logCreds = ReceiveMsg(m_messagingSocket);

    if (logCreds.messageKind == 2) {
        return true;
    }
    else {
        cerr << "logCreds credentials are invalid. Please try again." << endl << endl;
        return false;
    }
};
/*
NAME
        Client::ReceiveCredentials - Gets username and password
SYNOPSIS
        SOCKET Client::ReceiveCredentials();

DESCRIPTION
        We get the username and password from client and server

*/
void Client::ReceiveCredentials() {
    string username = "";
    string password = "";

    cout << "Enter thy username" << endl;
    cout << "Thou shalt be smiteth by the lamb of Moab if thou shalt go over 15 characters and useth the special characters." << endl;
    //we would like to keep continuing ad nauseam
    while (true) {
        getline(cin, username);

        if (username.size() > 0 && username.size() <= 16) {
            break;
        }
        else {
            cerr << "Thy username is a bit wonky.";
        }
    }
    //with the username found, back to business
    m_username = username;
    cout << "Thy Password (At most 250 characters) ";

    while (true) {
        getline(cin, password);

        if (password.size() > 0 && password.size() <= 250) {
            break;
        }
        else {
            cerr << "Thy password doth exceedeth the limit ";
        }
    }
    m_password = password;
}
/*
NAME
        Client::ClientMessageSending - Sends the data to the server.
SYNOPSIS
        bool ClientMessageSending(SOCKET userSocket, Message* userMessage);
        SOCKET userSocket -> The active socket on which to send
        Message* userMessage -> The message form that the user has specified in.
RETURNS
        Boolean of true for it arrived, if not then false
DESCRIPTION
        We send messages in this program

*/
void Client::MessageSending(SOCKET userSocket, Message* userMessage) {
    ConvertiumMessagum(userMessage);

    if (send(userSocket, userMessage->message, userMessage->length + MESSAGE_HEADER, 0) == SOCKET_ERROR) {
        cerr << "Send returned an error with error code: " << WSAGetLastError() << endl;
        closesocket(userSocket);
        exit(EXIT_FAILURE);
    }
}
/*
NAME
        void Client::ConvertiumMessagum() - Convert Message to data.
SYNOPSIS
        void Client::ConvertiumMessagum(Message* userMessage)
        Message* userMessage - a pointer to a userMessage

DESCRIPTION
        We convert raw data into systemic files in this progrom.

*/
void Client::ConvertiumMessagum(Message* userMessage) {
    int count = 0;
    strncpy(userMessage->message, userMessage->username.c_str(), userMessage->username.size());
    count += userMessage->username.size();
    userMessage->message[count] = '\0';
    count++;
    while (count < 20) 
    {
        userMessage->message[count] = '0';
        count++;
    }
    userMessage->message[count] = userMessage->messageKind + 48;
    count++;
    string length = to_string(userMessage->length);
    for (int i = 3 - length.size(); i > 0; i--) {
        userMessage->message[count] = '0';
        count++;
    }
    strncpy(&userMessage->message[count], length.c_str(), length.size());
    count += length.size();
    strncpy(&userMessage->message[count], userMessage->content.c_str(), userMessage->content.size());
}
/*
NAME
        Client::ReceiveMsg - Receive the data from the server.
SYNOPSIS
        bool ClientMessageSending(SOCKET userSocket, Message* userMessage);
        SOCKET userSocket -> The active socket
        
RETURNS
        The client data in message form
DESCRIPTION
        We receive messages in this program

*/
Client::Message Client::ReceiveMsg(SOCKET userSocket) {
    Message recvMessage;
    char headerbuff[MESSAGE_HEADER];
    char messagebuff[MESSAGE_LENGTH];
    int tnb = 0;
    int nb = 0;
    int length = 0;
    while (tnb < MESSAGE_HEADER) {
        nb = recv(userSocket, &headerbuff[tnb], MESSAGE_HEADER - tnb, 0);
        if (nb == 0) {
            cerr << "Connection Closed" << endl;
            closesocket(userSocket);
            exit(EXIT_FAILURE);
        }
        else if (nb == SOCKET_ERROR) {
            cerr << "Recv error: " << WSAGetLastError() << endl;
            closesocket(userSocket);
            exit(EXIT_FAILURE);
        }
        tnb += nb;
        if (tnb == MESSAGE_HEADER) {
            char temp[3];
            memcpy(temp, &headerbuff[20], 3);
            length = atoi(temp);
        }
    }
    nb = 0;
    tnb = 0;
    while (tnb < length) {
        nb = recv(userSocket, &messagebuff[tnb], MESSAGE_LENGTH - tnb, 0);
        if (nb == 0) {
            cerr << "Server closed" << endl;
            closesocket(userSocket);
            exit(EXIT_FAILURE);
        }
        else if (nb == SOCKET_ERROR) {
            cerr << "Recv returned an error with error code: " << WSAGetLastError() << endl;
            closesocket(userSocket);
            exit(EXIT_FAILURE);
        }
        tnb += nb;
    }
    int i = 0;
    while (headerbuff[i] != '\0') {
        recvMessage.username += headerbuff[i];
        i++;
    }
    recvMessage.messageKind = headerbuff[20] - 48;
    recvMessage.length = length;
    recvMessage.content.append(messagebuff, length);
    memcpy(recvMessage.message, headerbuff, MESSAGE_HEADER);
    memcpy(&recvMessage.message[MESSAGE_HEADER], messagebuff, length);
    return recvMessage;
}
/*
NAME
        void Client::ServerToDisplay() - Launch the server process
SYNOPSIS
        void Client::ServerToDisplay()

DESCRIPTION
        This sends directly to the display server

*/
void Client::ServerToDisplay() {
    Message tempMessage;

    while (true) {
        tempMessage = ReceiveMsg(m_messagingSocket);
        MessageSending(m_displayingSocket, &tempMessage);
    }
}

/*
NAME
        void Client::LaunchDisplay() - Launch the server process
SYNOPSIS
        void Client::LaunchDisplay(Message* userMessage)

DESCRIPTION
        We open the displaychat process from here

*/

void Client::LaunchDisplay() {
    //Your code from the website, professor
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));

    si.cb = sizeof(si);

    ZeroMemory(&pi, sizeof(pi));


    if (CreateProcess("C:\\Users\\Sanskar Maratha\\Desktop\\network_final_chatroom\\network_final_chatroom\\Client\\DisplayChat\\Delease\\DisplayChat.exe", NULL, NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi,
                       NULL, NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi) )

    {

        cout << "DisplayChat opened up in debug" << endl;

    }
	if (CreateProcess("C:\\Users\\Sanskar Maratha\\Desktop\\network_final_chatroom\\network_final_chatroom\\Client\\DisplayChat\\Release\\DisplayChat.exe", NULL, NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	{
		cout << "DisplayChat opened up in release" << endl;
	}
}

/*
NAME
        void Client::ClientToServer() - The beginning of the messages
SYNOPSIS
        void Client::ClientToServer()

DESCRIPTION
        We start talkign to the display and server

*/
void Client::ClientToServer() {
    cout << "Thou hast entered the chatroom" << endl;
    Sleep(1000);
    cout << "Very good. Send messages now." << endl;

    string input;
    Message message;
    message.username = m_username;
    message.messageKind = 1;

    while (true) {

        getline(cin, input);

        if (input.size() > 0 && input.size() <= 280) {
            message.length = input.size();
            message.content = input;

            MessageSending(m_messagingSocket, &message);

            cout << endl;
        }
        else {
            cerr << "Message is empty or exceeded max length. Try again." << endl;
        }
    }
}

/*
NAME
        Client::~Client()
SYNOPSIS
        This basically ends the server

DESCRIPTION
        Brave death. Took one for the team.
*/
Client::~Client() {
    cout << "We thanketh thee for a remarkable semester!" << endl;
}