#include <iostream>
#include "Client.h"
#include <thread>
using namespace std;

unsigned __stdcall WaitForUserInput(void* p_client) {
	Client* chat = (Client*)p_client;

	chat->ClientToServer();

	return 0;
}

int main() {
	Client moiChat;
	//I consulted with stackoverflow and they said that having threads would be the best way to deal with them!
	if (_beginthreadex(0, 0, WaitForUserInput, (void*)&moiChat, 0, 0) == 0) {
		cerr << "Thread start error." << endl;
		exit(1);
	}

	moiChat.ServerToDisplay();
}
