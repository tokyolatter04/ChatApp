
#include <iostream>//

#include "../include/protocol.hpp"
#include "../include/cryptography.hpp"
#include "../include/utils.hpp"

//////////////////////////////////////
/////////// PACKET HEADER ////////////
//////////////////////////////////////

/*
	Parse the packet header from a JSON string
	Return false if any errors occur while parsing
*/

bool PacketHeaderData::ParseJson(PacketHeaderData* header_data, std::string json) {

	// Load the JSON string

	JsonHandler jsHandler = soft_create_handler();
	JsonValue jsHeaderVal = soft_load_string(&jsHandler, (char*)json.c_str());

	if (jsHandler.error.exists) {
		// Error occurred while parsing the JSON data

		return false;
	}

	if (jsHeaderVal.type != JSON_DATA_TYPE_OBJECT) {
		// Header is not the correct data type

		return false;
	}

	JsonObject jsHeaderObj = jsHeaderVal.data._object;

	// Get the packet ID

	JsonValue* jsPacketIDVal;

	if (!json_object_get_value(&jsHeaderObj, "id", &jsPacketIDVal)) {
		// Header object does not contain a packet ID

		return false;
	}

	if (jsPacketIDVal->type != JSON_DATA_TYPE_STRING) {
		// Packet ID is not the correct data type

		return false;
	}

	JsonString jsPacketIDStr = jsPacketIDVal->data._string;

	// Get the body hash

	JsonValue* jsPacketBodyHashVal;

	if (!json_object_get_value(&jsHeaderObj, "body-hash", &jsPacketBodyHashVal)) {
		// Header object does not contain a body hash

		return false;
	}

	if (jsPacketBodyHashVal->type != JSON_DATA_TYPE_STRING) {
		// Body hash is not the correct data type

		return false;
	}

	JsonString jsPacketBodyHashStr = jsPacketBodyHashVal->data._string;

	// Get the body size

	JsonValue* jsPacketBodySizeVal;

	if (!json_object_get_value(&jsHeaderObj, "body-size", &jsPacketBodySizeVal)) {
		// Header object does not contain a body size

		return false;
	}

	if (jsPacketBodySizeVal->type != JSON_DATA_TYPE_INT) {
		// Body size is not the correct data type

		return false;
	}

	JsonInt jsPacketBodySizeInt = jsPacketBodySizeVal->data._int;

	// Get the packet channel

	JsonValue* jsPacketChannelVal;

	if (!json_object_get_value(&jsHeaderObj, "channel", &jsPacketChannelVal)) {
		// Header object does not contain a channel

		return false;
	}

	if (jsPacketChannelVal->type != JSON_DATA_TYPE_STRING) {
		// Channel is not the correct data type

		return false;
	}

	JsonString jsPacketChannelStr = jsPacketChannelVal->data._string;

	// Get the packet flags

	JsonValue* jsPacketFlagsVal;

	if (!json_object_get_value(&jsHeaderObj, "flags", &jsPacketFlagsVal)) {
		// Header object does not contain flags

		return false;
	}

	if (jsPacketFlagsVal->type != JSON_DATA_TYPE_LIST) {
		// Flags are not the correct data type

		return false;
	}

	JsonList jsPacketFlagsList = jsPacketFlagsVal->data._list;

	// Output

	*header_data = PacketHeaderData();

	header_data->header_val = jsHeaderVal;
	header_data->id = jsPacketIDStr;
	header_data->body_hash = jsPacketBodyHashStr;
	header_data->body_size = jsPacketBodySizeInt;
	header_data->channel = jsPacketChannelStr;
	header_data->flags = jsPacketFlagsList;

	return true;
}

/////////////////////////////////
/////////// PROTOCOL ////////////
/////////////////////////////////

/*
	Write bytes to the packet decoder buffer
	Continue packet decoding until a complete packet has been formed, add this to the queue
	Use hashing the determine if the packet body is correct
*/

void Protocol::PushBytes(std::string data) {

	for (char chr : data) {

		buffer << chr;

		if (parse_mode == ParseMode::Header) {

			// Only check for the delimiter if the current character is a delimiter char

			if (chr == PACKET_SPLITTER_CHAR) {

				// Check if the last 5 characters of the buffer are equal to the packet splitter

				std::string last_5 = buffer.str().substr(buffer.str().length() - 5);

				if (last_5 == PACKET_SPLITTER) {

					// Delimiter has been found indicating the header has been built

					// Header is equal to the buffer minus the last 5 delimiter characters

					std::string header = buffer.str().substr(0, buffer.str().length() - 5);

					// Parse the JSON header

					PacketHeaderData header_data;

					if (!PacketHeaderData::ParseJson(&header_data, header)) {
						// Error parsing JSON header

						std::cout << "Header Parse Error\n";
						std::cout << "Header: " << header << '\n';

						while (true);
					}

					std::cout << "ID: " << header_data.id << '\n';
					std::cout << "Body Hash: " << header_data.body_hash << '\n';
					std::cout << "Body Size: " << header_data.body_size << '\n';
					std::cout << "Channel: " << header_data.channel << '\n';

					// Fill current packet with header data

					current_packet.id = header_data.id;
					current_packet.body_hash = header_data.body_hash;
					current_packet.body_size = header_data.body_size;
					current_packet.channel = header_data.channel;

					for (uint64 i = 0; i < header_data.flags.length; i++) {

						JsonValue* jsFlagVal = json_list_get(&header_data.flags, i);

						if (jsFlagVal->type != JSON_DATA_TYPE_STRING) {
							// Flag is not the correct data type

							std::cout << "Flag Data Type Is Wrong\n";
							while (true);
						}

						std::cout << "Flag " << i << ": " << jsFlagVal->data._string << '\n';

						current_packet.flags.push_back(jsFlagVal->data._string);
					}

					// Set the parse mode to body and clear the buffer

					parse_mode = ParseMode::Body;
					buffer.str("");

					// Cleanup

					json_value_free(&header_data.header_val, TRUE);
				}
			}
		}
		else if (parse_mode == ParseMode::Body) {

			// Check if the packet body is complete

			if (buffer.tellp() == current_packet.body_size) {

				// Body is equal to the decrypted buffer

				std::string body = aes.Decrypt(buffer.str());

				// Check if the body hash is correct

				if (Cryptography::Sha256::ComputeTextHash(body) != current_packet.body_hash) {
					// Body hash is not correct

					std::cout << "Body SHA256 Checksum Failure!\n";
					while (true);
				}

				// Set body for the current packet

				current_packet.body = body;

				// Add packet to the packet queue

				packets.push(current_packet);

				// Reset current packet

				current_packet = RawPacket();

				// Set the parse mode back to header and clear the buffer

				parse_mode = ParseMode::Header;
				buffer.str("");
			}
		}
	}
}

/*
	Create a packet from the channel, body and flags
	Encode this packet into a format which can be sent across the network and decoded by the receiver
*/

std::string Protocol::BuildPacket(std::string channel, std::string body, std::vector<std::string> flags) {

	// Create packet meta data

	std::string encrypted_body = aes.Encrypt(body);

	std::string id = Utils::RandomUUID();
	std::string body_hash = Cryptography::Sha256::ComputeTextHash(body);
	uint64 body_size = encrypted_body.length();

	// Create flags

	JsonList jsFlagsList = json_create_list();

	for (std::string& flag : flags) {
		json_list_add(&jsFlagsList, json_create_string_value(flag.c_str()));
	}

	// Create JSON header

	JsonObject jsHeaderObj = json_create_object();

	json_object_add(&jsHeaderObj, "id", json_create_string_value(id.c_str()));
	json_object_add(&jsHeaderObj, "body-hash", json_create_string_value(body_hash.c_str()));
	json_object_add(&jsHeaderObj, "body-size", json_create_int_value(body_size));
	json_object_add(&jsHeaderObj, "channel", json_create_string_value(channel.c_str()));
	json_object_add(&jsHeaderObj, "flags", json_create_list_value(jsFlagsList));

	// Encode header

	JsonHandler jsHandler = soft_create_handler();
	char* header = soft_dump_string(&jsHandler, json_create_object_value(jsHeaderObj));

	// Cleanup

	json_object_free(&jsHeaderObj, FALSE);

	// Create packet

	std::ostringstream packet(std::ios::binary);

	packet << header;
	packet << PACKET_SPLITTER;
	packet << encrypted_body;

	return packet.str();
}

/*
	Read a packet from the queued packets
	Return false if there are no packets to dequeue
*/

bool Protocol::GetPacket(RawPacket* out_packet) {
	if (packets.empty()) {
		return false;
	}

	*out_packet = packets.front();
	packets.pop();

	return true;
}
