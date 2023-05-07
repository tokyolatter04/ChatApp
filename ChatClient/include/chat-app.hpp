
#pragma once

#include "../include/networking.hpp"

#undef SendMessage

class ChatUser {
public:
	std::string id;
	std::string name;

	ChatUser(std::string _id, std::string _name)
		: id(_id), name(_name) {}

	ChatUser() {}
};

class ChatMessage {
public:
	std::string id;
	std::string content;
	ChatUser sender;

	ChatMessage(std::string _id, std::string _content, ChatUser _sender)
		: id(_id), content(_content), sender(_sender) {}

	ChatMessage(std::string _content)
		: content(_content) {}

	ChatMessage() {}
};

class ChatApp {
private:
	TcpClient client;
public:
	// List of all of the currently connected users
	std::vector<ChatUser> users;

	// List of all of the messages
	std::vector<ChatMessage> messages;

	ChatApp() {}

	bool IsConnected() {
		return client.IsConnected();
	}

	// Start the chat app and connect to the server
	bool Start();

	// Shutdown the chat app
	void Shutdown();

	// Receive and process packets sent to this client
	void PacketProcessor();

	// Send a message to the server
	void SendMessage(ChatMessage message);
};
