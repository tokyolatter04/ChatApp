
#pragma once

#include <string>
#include <vector>

#include "../include/networking.hpp"

#undef SendMessage

class ChatMessage;

class ChatUser {
public:
	std::string id;
	std::string name;
	TcpClient client;

	ChatUser(std::string _id, std::string _name, TcpClient _client)
		: id(_id), name(_name), client(_client) {}

	ChatUser() {}

	// Send a message to this client
	void SendMessage(ChatMessage message);

	// Send a list of messages to this client (a list of all of the chat app's messages)
	void SendMessageList(std::vector<ChatMessage> messages);

	// Send a list of users to this client (a list of all of the connected users)
	void SendUserList(std::vector<ChatUser> users);
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
	TcpServer server;

	// List of all of the currently connected users
	std::vector<ChatUser> users;

	// List of all of the messages
	std::vector<ChatMessage> messages;

	// Receive and process packets sent from this client
	void ProcessPackets(ChatUser* user);
public:
	ChatApp();

	// Start the chat app
	void Start();

	// Shutdown the chat app
	void Shutdown();

	// Listen for connection requests
	void ConnectionListener();

	// Send a message to all of the clients
	void BroadcastMessage(ChatMessage message);

	// Send the list of connected users to all of the clients
	void BroadcastUsers();
};
