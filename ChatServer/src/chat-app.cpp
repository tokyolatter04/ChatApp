
#include <iostream>//

#include "../include/chat-app.hpp"
#include "../include/utils.hpp"

/***************************************
* ChatUser
 ***************************************/

void ChatUser::SendMessage(ChatMessage message) {

	client.SendPacket("message", "", { message.id, message.content, message.sender.id, message.sender.name });

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

			// Extract data from the message packet

			if (packet.flags.size() < 4) {
				// Packet does not contain any message content

				continue;
			}

			std::string content = packet.flags[1];

			// Build and store the message

			ChatMessage message = ChatMessage(
				Utils::RandomUUID(),
				content,
				*user
			);

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
