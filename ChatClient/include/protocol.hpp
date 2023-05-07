
#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <queue>

#include <SOFTJSON/softjson.hpp>

#include "cryptography.hpp"

/*
	Represents a packet that is transferred over the network
*/

class RawPacket {
public:
	std::string id;
	std::string body_hash;
	uint64 body_size;
	std::string channel;
	std::vector<std::string> flags;

	std::string body;
};

/*
	Represents a parsed JSON packet header
*/

class PacketHeaderData {
public:
	JsonValue header_val;

	JsonString id;
	JsonString body_hash;
	JsonInt body_size;
	JsonString channel;
	JsonList flags;

	static bool ParseJson(PacketHeaderData* header_data, std::string json);
};

class Protocol {
private:
	enum class ParseMode {
		Header,
		Body
	};

	Cryptography::Aes aes;

	std::ostringstream buffer;
	RawPacket current_packet;
	ParseMode parse_mode;

	std::queue<RawPacket> packets;

	const std::string PACKET_SPLITTER = "\t\t\t\t\t";
	const char PACKET_SPLITTER_CHAR = '\t';
public:

	Protocol(std::string aes_key)
		: aes(aes_key), buffer(std::ios::binary), parse_mode(ParseMode::Header) {}

	Protocol() {}

	Protocol(const Protocol& other)
		: buffer(other.buffer.str()), current_packet(other.current_packet), parse_mode(other.parse_mode) {}

	Protocol& operator=(const Protocol& other) {
		return (Protocol&)other;
	}

	// Write bytes to internal packet decoder buffer
	void PushBytes(std::string data);

	// Encode a packet that can be sent across the network
	std::string BuildPacket(std::string channel, std::string body, std::vector<std::string> flags);

	// Read a packet from the queued packets
	bool GetPacket(RawPacket* out_packet);
};
