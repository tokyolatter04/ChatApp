
#include <iostream>//
#include <thread>

#include "../include/chat-app.hpp"

bool ChatApp::Start() {
	// Setup and open client

	client = TcpClient("127.0.0.1", 5000);

	if (!client.Open()) {
		return false;
	}

	// Connect to the server

	client.Connect();

	// Initialise the encrypted communication

	if (!client.InitEncryption()) {
		return false;
	}

	// Start packet reader in a new thread

	std::thread([=]() {
		client.StartListening();
	}).detach();

	// Open packet processor in a new thread

	std::thread([=]() {
		PacketProcessor();
	}).detach();

	return true;
}

void ChatApp::Shutdown() {
	// Disconnect from the server

	client.Disconnect();
}

void ChatApp::PacketProcessor() {

	while (client.IsConnected()) {

		// Get packet

		RawPacket packet;

		if (!client.GetPacket(&packet)) {
			// Poll every 50ms

			Sleep(50);

			continue;
		}

		// Decode packet

		std::cout << "Packet: " << packet.body << '\n';

	}
}

void ChatApp::SendMessage(std::string content) {

	client.SendPacket("message", content);

}
