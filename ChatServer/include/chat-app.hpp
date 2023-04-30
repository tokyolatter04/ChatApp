
#pragma once

#include <string>
#include <vector>

#include "../include/networking.hpp"

class ChatUser {
public:
	std::string id;
	std::string name;
	TcpClient client;

	ChatUser(std::string _id, std::string _name, TcpClient _client)
		: id(_id), name(_name), client(_client) {}

	ChatUser() {}

	void SendMessage(std::string content);
};

class ChatMessage {
public:
	std::string id;
	std::string content;
	ChatUser sender;

	ChatMessage(std::string _id, std::string _content, ChatUser _sender)
		: id(_id), content(_content), sender(_sender) {}

	ChatMessage() {}
};

class ChatApp {
private:
	TcpServer server;

	std::vector<ChatUser> users;
	std::vector<ChatMessage> messages;

	void ProcessPackets(ChatUser* user);
public:
	ChatApp();

	void Start();
	void Shutdown();

	void ConnectionListener();
};
