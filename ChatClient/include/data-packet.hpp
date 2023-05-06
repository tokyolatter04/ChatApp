
#pragma once

#include "chat-app.hpp"

enum class DataPacketType {
	Message,
	MessageList
};

class DataPacket {
public:
	DataPacketType type;

	struct {
		ChatMessage message;
		std::vector<ChatMessage> message_list;
	} data;

	std::string Encode();

	static DataPacket CreateMessagePacket(ChatMessage message);
	static bool FromJSONData(DataPacketType type, std::string data, DataPacket* out_packet);
};
