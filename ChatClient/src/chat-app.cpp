
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

		if (packet.channel == "message") {

			// Extract data from the message packet

			if (packet.flags.size() < 4) {
				// Packet does not contain any message data

				continue;
			}

			std::string id = packet.flags[0];
			std::string content = packet.flags[1];
			std::string sender_id = packet.flags[2];
			std::string sender_name = packet.flags[3];

			// Build and store the message

			ChatMessage message = ChatMessage(
				id,
				content,
				ChatUser(sender_id, sender_name)
			);

			messages.push_back(message);
		}
	}
}

void ChatApp::SendMessage(ChatMessage message) {

	client.SendPacket("message", "", { "", message.content, "", "" });

}
