
#include <iostream>//
#include <thread>

#include "../include/chat-app.hpp"
#include "../include/data-packet.hpp"

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

			// Decode the packet

			DataPacket data_packet;

			if (!DataPacket::FromJSONData(DataPacketType::Message, packet.body, &data_packet)) {
				// Failed to decode the packet

				continue;
			}

			ChatMessage message = data_packet.data.message;

			// Store the message

			messages.push_back(message);
		}
		else if (packet.channel == "message-list") {
			// Decode packet

			DataPacket data_packet;

			if (!DataPacket::FromJSONData(DataPacketType::MessageList, packet.body, &data_packet)) {
				// Failed to decode the packet

				continue;
			}

			// Store the message list

			messages = data_packet.data.message_list;
		}
		else if (packet.channel == "user-list") {
			// Decode packet

			DataPacket data_packet;

			if (!DataPacket::FromJSONData(DataPacketType::UserList, packet.body, &data_packet)) {
				// Failed to decode the packet

				continue;
			}

			// Store the user list

			users = data_packet.data.user_list;
		}
	}
}

void ChatApp::SendMessage(ChatMessage message) {

	DataPacket packet = DataPacket::CreateMessagePacket(message);
	std::string encoded = packet.Encode();

	client.SendPacket("message", encoded);
}
