
#include <iostream>

#include "include/networking.hpp"

int main(void) {

	// Intiialise WinSock

	WinSockInit();

	// Setup server

	TcpServer server = TcpServer("127.0.0.1", 5000);

	server.Open();

	while (true) {
		TcpClient client;

		std::cout << "Waiting for client...\n";

		if (server.Accept(&client)) {
			std::cout << "New client connected!\n";

			if (client.InitEncrytion()) {
				std::cout << "Init Encryption worked\n";
			}
			else {
				std::cout << "Init Encryption failed\n";
			}
		}
		else {
			std::cout << "Failed to accept new connection\n";
			Sleep(500);
		}
	}

}
