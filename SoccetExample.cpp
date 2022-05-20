#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "7000"
const int size = 2000;
#include<Winsock2.h>
#include <ws2tcpip.h>
#include<iostream>
using namespace std;

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    int iResult;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = WSAStartup(MAKEWORD(1, 1), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }



    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET ConnectSocket /*= INVALID_SOCKET*/;
    ptr = result;

    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
        ptr->ai_protocol);
    printf("ConnectSocket value: %ld\n", ConnectSocket);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }


    int recvbuflen = DEFAULT_BUFLEN;
    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];


    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    printf("Bytes Sent: %ld\n", iResult);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("Bytes Sent: %ld\n", iResult);
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    do {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
    } while (iResult > 0);
    //Miller's code
    char buff[2000];
    char rbuff[2000];
    for (int i = 0; i < 1000; i++) {

        //rv = sent();
        //if(rv==error){}
        //recv();
    }
    return 0;
}