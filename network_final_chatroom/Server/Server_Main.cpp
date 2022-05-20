
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

int main() {
	Server myLocalServer;
	while (true) {
		int client_pos = myLocalServer.AcceptNewClient();
		if (client_pos < 0) return 1;
		myLocalServer.GoTalkToYourRelatives();
	}
}