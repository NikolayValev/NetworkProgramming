#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
const int m_size = 2000;
#include<Winsock2.h>
#include<iostream>
#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

//Loading Bar defines
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void loadingBar(double input) {
    double percentage = input / 1000;
    int val = (int)(percentage * 100);
    int lpad = (int)(percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}
static SOCKET connectTCP(const char* host, const char* service) {

    struct hostent* st_host;
    struct servent* st_service;
    struct sockaddr_in sin;
    SOCKET ConnectSocket;

    memset((char*)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    if ((st_service = getservbyname(service, "tcp")) != NULL) {
        sin.sin_port = st_service->s_port;
        printf("port No. %ld\n", (long int)ntohs(st_service->s_port));
    }
    else if ((sin.sin_port = htons((short int)atoi(service))) == 0)
    {
        cerr << "Can't get the port number of the requested service.\n";
        exit(1);
    }
    st_host = gethostbyname(host);
    if (st_host != NULL)
    {
        memcpy((char*)&sin.sin_addr, st_host->h_addr, st_host->h_length);
    }
    else if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
    {
        printf("Invalid host address\n");
        exit(1);
    }
    //TODO(nvalev) Add it in dotted notation as well.
    printf("IP address = 0x%x\n", sin.sin_addr.s_addr);

    ConnectSocket = socket(PF_INET, SOCK_STREAM, 0);
    printf("ConnectSocket value: %ld\n", ConnectSocket);
    if (ConnectSocket == INVALID_SOCKET)

    {
        perror("Could not allocate socket");
        exit(1);
    }
    if (connect(ConnectSocket, (struct sockaddr*) & sin, sizeof(sin)) == SOCKET_ERROR)
    {
        perror("Attempt to connect to server failed");
        exit(1);
    }
    return(ConnectSocket);
}


int main(int argc, char* argv[]) {

    srand(time(NULL));

    int bytesPartialRead;
    int totalBytesToRead;
    double totalTime = 0;
    char buffRead[m_size + 1];
    char buffWrite[m_size];
    std::chrono::high_resolution_clock::time_point begin, end;
    string host = "cs.ramapo.edu";
    string service = "echo";
    WORD wRequestedVersion = MAKEWORD(1, 1);
    WSADATA wsaData;

    int err = WSAStartup(wRequestedVersion, &wsaData);

    if (err != 0) {
        perror("Error: ");
        //cerr << "Could not find useable winsock.dll" << endl;
        return 1;
    }
    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
    {
        cerr << "Could not find useable DLL" << endl;
        WSACleanup();
        return 1;
    }


    SOCKET ConnectSocket = connectTCP(host.c_str(), service.c_str());

    for (int i = 0; i < 1000; i++) {
        loadingBar(i);
        for (int j = 0; j < m_size; j++) {
            buffWrite[j] = rand() % 10;
        }
        begin = std::chrono::high_resolution_clock::now();
        send(ConnectSocket, buffWrite, m_size, 0);
        perror("Error: ");
        bytesPartialRead = 0;
        totalBytesToRead = 0;

        while (totalBytesToRead < m_size) {
            bytesPartialRead = recv(ConnectSocket, &buffRead[totalBytesToRead], m_size - totalBytesToRead, 0);
            totalBytesToRead += bytesPartialRead;
            if (bytesPartialRead < 0 || totalBytesToRead < 0) {
                perror("Error: ");
            }
        }
        end = std::chrono::high_resolution_clock::now();
        if (totalBytesToRead == 0) {
            printf("Disconnect on recv");
            exit(1);
        }
        std::chrono::high_resolution_clock::duration time = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin);

        totalTime += time.count();

        buffRead[m_size] = '\0';

    }
    cout << endl << "Total Time: " << totalTime / 1000000000 << " seconds." << endl;
    return 0;
}#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
const int m_size = 2000;
#include<Winsock2.h>
#include<iostream>
#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

//Loading Bar defines
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void loadingBar(double input) {
    double percentage = input / 1000;
    int val = (int)(percentage * 100);
    int lpad = (int)(percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}
static SOCKET connectTCP(const char* host, const char* service) {

    struct hostent* st_host;
    struct servent* st_service;
    struct sockaddr_in sin;
    SOCKET ConnectSocket;

    memset((char*)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    if ((st_service = getservbyname(service, "tcp")) != NULL) {
        sin.sin_port = st_service->s_port;
        printf("port No. %ld\n", (long int)ntohs(st_service->s_port));
    }
    else if ((sin.sin_port = htons((short int)atoi(service))) == 0)
    {
        cerr << "Can't get the port number of the requested service.\n";
        exit(1);
    }
    st_host = gethostbyname(host);
    if (st_host != NULL)
    {
        memcpy((char*)&sin.sin_addr, st_host->h_addr, st_host->h_length);
    }
    else if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
    {
        printf("Invalid host address\n");
        exit(1);
    }
    //TODO(nvalev) Add it in dotted notation as well.
    printf("IP address = 0x%x\n", sin.sin_addr.s_addr);

    ConnectSocket = socket(PF_INET, SOCK_STREAM, 0);
    printf("ConnectSocket value: %ld\n", ConnectSocket);
    if (ConnectSocket == INVALID_SOCKET)

    {
        perror("Could not allocate socket");
        exit(1);
    }
    if (connect(ConnectSocket, (struct sockaddr*) & sin, sizeof(sin)) == SOCKET_ERROR)
    {
        perror("Attempt to connect to server failed");
        exit(1);
    }
    return(ConnectSocket);
}


int main(int argc, char* argv[]) {

    srand(time(NULL));

    int bytesPartialRead;
    int totalBytesToRead;
    double totalTime = 0;
    char buffRead[m_size + 1];
    char buffWrite[m_size];
    std::chrono::high_resolution_clock::time_point begin, end;
    string host = "cs.ramapo.edu";
    string service = "echo";
    WORD wRequestedVersion = MAKEWORD(1, 1);
    WSADATA wsaData;

    int err = WSAStartup(wRequestedVersion, &wsaData);

    if (err != 0) {
        perror("Error: ");
        //cerr << "Could not find useable winsock.dll" << endl;
        return 1;
    }
    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
    {
        cerr << "Could not find useable DLL" << endl;
        WSACleanup();
        return 1;
    }


    SOCKET ConnectSocket = connectTCP(host.c_str(), service.c_str());

    for (int i = 0; i < 1000; i++) {
        loadingBar(i);
        for (int j = 0; j < m_size; j++) {
            buffWrite[j] = rand() % 10;
        }
        begin = std::chrono::high_resolution_clock::now();
        send(ConnectSocket, buffWrite, m_size, 0);
        perror("Error: ");
        bytesPartialRead = 0;
        totalBytesToRead = 0;

        while (totalBytesToRead < m_size) {
            bytesPartialRead = recv(ConnectSocket, &buffRead[totalBytesToRead], m_size - totalBytesToRead, 0);
            totalBytesToRead += bytesPartialRead;
            if (bytesPartialRead < 0 || totalBytesToRead < 0) {
                perror("Error: ");
            }
        }
        end = std::chrono::high_resolution_clock::now();
        if (totalBytesToRead == 0) {
            printf("Disconnect on recv");
            exit(1);
        }
        std::chrono::high_resolution_clock::duration time = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin);

        totalTime += time.count();

        buffRead[m_size] = '\0';

    }
    cout << endl << "Total Time: " << totalTime / 1000000000 << " seconds." << endl;
    return 0;
}