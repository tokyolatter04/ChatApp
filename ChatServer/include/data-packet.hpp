
#pragma once

#include "chat-app.hpp"

/*
	Message: Data packet contains data for a single message
	MessageList: Data packet contains data for a list of messages
	UserList: Data packet contains data for a list of users
*/

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

	// Encode a data packet into a JSON string
	std::string Encode();

	// Create a data packet with message data
	static DataPacket CreateMessagePacket(ChatMessage message);

	// Create a data packet with message list data
	static DataPacket CreateMessageListPacket(std::vector<ChatMessage> messages);

	// Create a data packet with user list data
	static DataPacket CreateUserListPacket(std::vector<ChatUser> users);

	// Parse a data packet from a JSON string
	static bool FromJSONData(DataPacketType type, std::string data, DataPacket* out_packet);
};
