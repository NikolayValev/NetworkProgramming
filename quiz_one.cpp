/*
Lab 1 10 / 1 / 2020
Use the echo service to determine how long it would take
to send and receive 1000 messages of 2000 bytes from a remote
site.Receive each message fully before sending the next one.
I will give some extra credit  Do something about my example
using perror in your code.
*/
#define DEFAULT_BUFLEN 512
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "27015"
using namespace std;

int main(int argc, char *argv[])
{
  WSADATA wsaData;
  int iResult;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0)
  {
    printf("WSAStartup failed: %d\n", iResult);
    return 1;
  }

  struct addrinfo *result = NULL,
                  *ptr = NULL,
                  hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port
  iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
  if (iResult != 0)
  {
    printf("getaddrinfo failed: %d\n", iResult);
    WSACleanup();
    return 1;
  }

  SOCKET ConnectSocket = INVALID_SOCKET;
  // Attempt to connect to the first address returned by the call to getaddrinfo
  ptr = result;

  // Create a SOCKET for connecting to server
  ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                         ptr->ai_protocol);

  if (ConnectSocket == INVALID_SOCKET)
  {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return 1;
  }

  iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
  if (iResult == SOCKET_ERROR)
  {
    closesocket(ConnectSocket);
    ConnectSocket = INVALID_SOCKET;
  }
  
  return 0;
}