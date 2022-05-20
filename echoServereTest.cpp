/**
 * @author Nikolay Valev
 * @brief Program to send and receive from the echo server and test the time
 * @date 10/08/2020
*/
#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
const int LINESZ = 2000;
#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

#include <Winsock2.h>
static SOCKET connectTCP(const char* host, const char* service);
void showMessage(int num);
int main(int argc, char* argv[])
{
    //To make sure we seed the random number generator to be as random as we can
    srand(time(NULL));
    // First ensure connection
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    string allData;

    // A macro to create a word with low order and high order bytes as specified.
    wVersionRequested = MAKEWORD(1, 1);

    // Test that version 1.1 is available.
    err = WSAStartup(wVersionRequested, &wsaData);

    if (err != 0) {

        // Tell the user that we couldn't find a useable winsock.dll. */
        cerr << "Could not find useable DLL" << endl;
        return 1;
    }

    // Confirm that the Windows Sockets DLL supports version 1.1.
    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
    {
        // Tell the user that we couldn't find a useable winsock DLL.
        WSACleanup();  // Terminates the use of the
        cerr << "Could not find useable DLL" << endl;
        return 1;
    }


    int i = 0;
    int bytesReadOnce;         /* The number of bytes read from a single read. */
    int totalBytesRead;        /* The total number of bytes read. */
    char inRead[LINESZ + 1];  /* Response data from the server. */
    char toWrite[LINESZ]; /* Data to be sent to the server. */
    std::chrono::high_resolution_clock::time_point begin, end; /* Containers to record time*/
    std::chrono::high_resolution_clock::duration time; /* Container to record the time taken for each iteration*/
    double totalTime = 0;
    SOCKET our_socket;
    string host = "cs.ramapo.edu"; /* Since we are using the college server, might as well hard code it.*/
    string service = "echo";
    our_socket = connectTCP(host.c_str(), service.c_str()); // Connecting to the server.
    for (int i = 0; i < 1000; i++)
    {
        //Showing message to indicate progress
        showMessage(i + 1);
        //We randomize the data to be sent to the server.
        for (int j = 0; j < LINESZ; j++)
        {
            toWrite[j] = rand()%10;
        }
        try
        {
            //Start noting the time
            begin = std::chrono::high_resolution_clock::now();
            send(our_socket, toWrite, LINESZ, 0);

            bytesReadOnce = 0;
            totalBytesRead = 0;
            //Since it is possible that we don't receive all the data in one go, we receive in a loop until we read everything we sent.
            while (totalBytesRead < LINESZ)
            {
                bytesReadOnce = recv(our_socket, &inRead[totalBytesRead], LINESZ - totalBytesRead, 0);
                totalBytesRead += bytesReadOnce;
            }

            end = std::chrono::high_resolution_clock::now();
            time = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin);
            //Recording the total time taken until now.
            totalTime += time.count();
            /* If there was an error on the read, report it. */
            if (bytesReadOnce < 0)
            {
                printf("recv failed: %d\n", WSAGetLastError());
                return 1;
            }
            if (totalBytesRead == 0)
            {
                printf("Disconnect on recv");
            }
            inRead[LINESZ] = '\0';
        }
        catch (...)
        {
            printf("There was some kind of an error.");
            return 1;
        }

    }

    std::cout << "The total time taken was " << totalTime / 1000000000 << " seconds." << endl;
    return 0;
}


/**/
/*
NAME

    connectTCP - to make a connection to a TCP/IP server.

SYNOPSIS
*/
static SOCKET connectTCP(const char* host, const char* service)
/*
DESCRIPTION

    The "connectTCP" function will connect this client to a TCP/IP server.
    "host" is the name or IP address of the system on which the server
    resides.  "service" is the name or port number of the service to be
    accessed.

RETURNS

    This function returns the file descriptor of the connected socket
    if it is successful and does not return if it fails.  The error
    is displayed in the latter case.
*/
/**/
{
    struct hostent* he; /* Information about the remote host. */
    struct servent* se; /* Information about the requested service. */
    struct sockaddr_in sin; /* Internet end point address.  How do I find the format of this struct? */
    SOCKET s;              /* File descriptor for the connected socket. */


    /* Create an endpoint address for the server.   Structure for address
     * found in: netinet/in.h */
    memset((char*)&sin, 0, sizeof(sin)); /* sets the address to zero. */
    sin.sin_family = AF_INET;  /* Record the fact that we are using the
                                * internet family of addresses. */

                                /* Get the portnumber.  Note: services found in /etc/services. */
    if ((se = getservbyname(service, "tcp")) != NULL)
    {
        sin.sin_port = se->s_port;
        printf("port No. %ld\n", (long int)ntohs(se->s_port));
    }
    else if ((sin.sin_port = htons((short int)atoi(service)))
        == 0)
    {
        printf("Can't get the port number of the service.\n");
        exit(1);
    }

    /* Get the address of the remote host.  The host's name is looked up
     * on a name server. */
    if ((he = gethostbyname(host)) != NULL)
    {
        memcpy((char*)&sin.sin_addr, he->h_addr, he->h_length);
    }
    else if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
    {
        printf("Invalid host address\n");
        exit(1);
    }
    // Display the IP address in hexadecimal.  Let's convert it to dotted
    // notation in class.
    printf("IP address = 0x%x\n", sin.sin_addr.s_addr);

    /* Allocate a socket from which the connection to the server will be
     * be made.  Note: by setting the third argument to 0, means that we
     * are using the default prtocol (since there is usually one choice,
     * 0 seems like a good choice for the argument. */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        perror("Could not allocate socket");
        exit(1);
    }
    /* Connect to server. */
    if (connect(s, (struct sockaddr*) & sin, sizeof(sin)) == SOCKET_ERROR)
    {
        perror("Attempt to connect to server failed");
        exit(1);
    }
    /* Return the file descriptor for the connected socket. */
    return(s);
}
/**/
/*
NAME

    showMessage - to show a message to indicate progress.

SYNOPSIS
*/
void showMessage(int num)
/*
DESCRIPTION

    The function showMessage will send a message to the client to indicate progress

RETURNS

    This function returns nothing
*/
/**/
{
    if (num == 200)
    {
        std::cout << "Just got started! 20 percent done." << endl;
    }
    else if (num == 400)
    {
        std::cout << "We're getting there. 40 percent done." << endl;
    }
    else if (num == 600)
    {
        std::cout << "Just crossed half of all messages to be sent and received!" << endl;
    }
    else if (num == 800)
    {
        std::cout << "Hold your horses! 80 percent done." << endl;
    }
    else if (num == 1000)
    {
        std::cout << "Done with the 1000 messages!" << endl;
    }
}