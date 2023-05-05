
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
	std::vector<ChatUser> users;
	std::vector<ChatMessage> messages;

	ChatApp() {}

	bool Start();
	void Shutdown();

	void PacketProcessor();

	void SendMessage(ChatMessage message);
};
