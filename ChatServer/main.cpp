
#include <iostream>
#include <thread>
#include <vector>

#include "include/networking.hpp"

int main(void) {

	// Intiialise WinSock

	WinSockInit();

	// Setup server

	TcpServer server = TcpServer("127.0.0.1", 5000);

	server.Open();

	std::vector<TcpClient> clients;


	while (true) {
		TcpClient client;

		std::cout << "Waiting for client...\n";

		if (server.Accept(&client)) {
			std::cout << "New client connected!\n";

			if (client.InitEncrytion()) {
				std::cout << "Init Encryption worked\n";

				std::thread t1(&TcpClient::StartListening, &client);
				std::thread t2(&TcpServer::PacketListener, &server, &client);



				while (true) {
					Sleep(100);
				}
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
