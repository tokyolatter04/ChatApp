
#pragma once

#include "../include/networking.hpp"

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

	ChatMessage() {}
};

class ChatApp {
private:
	TcpClient client;

	std::vector<ChatUser> users;
	std::vector<ChatMessage> messages;
public:
	ChatApp() {}

	bool Start();
	void Shutdown();

	void PacketProcessor();

	void SendMessage(std::string content);
};
