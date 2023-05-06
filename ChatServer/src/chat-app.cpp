
#include <iostream>//

#include "../include/chat-app.hpp"
#include "../include/data-packet.hpp"
#include "../include/utils.hpp"

/***************************************
* ChatUser
 ***************************************/

void ChatUser::SendMessage(ChatMessage message) {

	DataPacket packet = DataPacket::CreateMessagePacket(message);
	std::string encoded = packet.Encode();

	client.SendPacket("message", encoded);
}

void ChatUser::SendMessageList(std::vector<ChatMessage> messages) {

	DataPacket packet = DataPacket::CreateMessageListPacket(messages);
	std::string encoded = packet.Encode();

	client.SendPacket("message-list", encoded);
}

/***************************************
* ChatApp
 ***************************************/

ChatApp::ChatApp() {
	// Pre-allocate max user count

	users.reserve(128);
}

void ChatApp::Start() {
	// Create & open server

	server = TcpServer("127.0.0.1", 5000);

	server.Open();
}

void ChatApp::Shutdown() {
	// Disconnect all clients

	for (ChatUser user : users) {
		user.client.Disconnect();
	}

	// Shutdown the server

	server.Shutdown();
}

void ChatApp::ConnectionListener() {

	while (server.IsOpen()) {

		// Attempt to accept a connection

		TcpClient client;

		if (!server.Accept(&client)) {
			// Error accepting connection

			Sleep(50);

			continue;
		}

		// Initialise the encrypted communication

		client.InitEncrytion();

		// Create ChatUser

		std::string id = Utils::RandomUUID();
		std::string name = "User-" + std::to_string(users.size());

		ChatUser user = ChatUser(id, name, client);

		// Add user to the user list

		users.push_back(user);

		// Send the list of current messages

		user.SendMessageList(messages);

		// Get a pointer to this user in the user list

		ChatUser* ptr = &users.back();

		// Start the client's packet reader in a new thread

		std::thread([=]() {
			ptr->client.StartListening();
		}).detach();

		// Pass pointer to the packet processor in a new thread

		std::thread([=]() {
			ProcessPackets(ptr);
		}).detach();
	}
}

void ChatApp::ProcessPackets(ChatUser* user) {

	while (user->client.IsConnected()) {

		// Get packet

		RawPacket packet;

		if (!user->client.GetPacket(&packet)) {
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

			// Assign the message ID and user object

			message.id = Utils::RandomUUID();
			message.sender = *user;

			// Store the message

			messages.push_back(message);

			// Broadcast the message

			BroadcastMessage(message);
		}
	}
}

void ChatApp::BroadcastMessage(ChatMessage message) {

	for (ChatUser& user : users) {
		user.SendMessage(message);
	}

}
