
#pragma once

#include "chat-app.hpp"

enum class DataPacketType {
	Message,
	MessageList,
	UserList
};

class DataPacket {
public:
	DataPacketType type;

	struct {
		ChatMessage message;
		std::vector<ChatMessage> message_list;
		std::vector<ChatUser> user_list;
	} data;

	std::string Encode();

	static DataPacket CreateMessagePacket(ChatMessage message);
	static DataPacket CreateMessageListPacket(std::vector<ChatMessage> messages);
	static DataPacket CreateUserListPacket(std::vector<ChatUser> users);
	static bool FromJSONData(DataPacketType type, std::string data, DataPacket* out_packet);
};
