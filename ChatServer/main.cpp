
#include <iostream>
#include <thread>
#include <vector>

#include "include/chat-app.hpp"

int main(void) {

	// Intiialise WinSock

	WinSockInit();

	// Setup Chat App

	ChatApp chat;

	chat.Start();

	chat.ConnectionListener();

}
