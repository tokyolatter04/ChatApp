
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

	void SendMessage(ChatMessage message);
	void SendMessageList(std::vector<ChatMessage> messages);
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

	std::vector<ChatUser> users;
	std::vector<ChatMessage> messages;

	void ProcessPackets(ChatUser* user);
public:
	ChatApp();

	void Start();
	void Shutdown();

	void ConnectionListener();

	void BroadcastMessage(ChatMessage message);
};
