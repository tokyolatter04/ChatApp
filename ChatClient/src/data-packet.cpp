
#include "../include/data-packet.hpp"

#include <SOFTJSON/softjson.hpp>

/***************************************
* Internal Helpers
 ***************************************/

bool ReadObject(JsonObject jsObj, std::string key, JsonObject* out_jsObj) {

	JsonValue* jsVal;

	if (!json_object_get_value(&jsObj, key.c_str(), &jsVal)) {
		// Failed to get the object

		return false;
	}

	if (jsVal->type != JSON_DATA_TYPE_OBJECT) {
		// Object is not the correct data type

		return false;
	}

	*out_jsObj = jsVal->data._object;

	return true;
}

bool ReadList(JsonObject jsObj, std::string key, JsonList* out_jsList) {

	JsonValue* jsVal;

	if (!json_object_get_value(&jsObj, key.c_str(), &jsVal)) {
		// Failed to get the list

		return false;
	}

	if (jsVal->type != JSON_DATA_TYPE_LIST) {
		// List is not the correct data type

		return false;
	}

	*out_jsList = jsVal->data._list;

	return true;
}

bool ReadString(JsonObject jsObj, std::string key, JsonString* out_jsStr) {

	JsonValue* jsVal;

	if (!json_object_get_value(&jsObj, key.c_str(), &jsVal)) {
		// Failed to get the string

		return false;
	}

	if (jsVal->type != JSON_DATA_TYPE_STRING) {
		// String is not the correct data type

		return false;
	}

	*out_jsStr = jsVal->data._string;

	return true;
}

bool ParseMessageObject(JsonObject jsObj, ChatMessage* out_message) {
	// Get the sender object

	JsonObject jsSenderObj;

	if (!ReadObject(jsObj, "sender", &jsSenderObj)) {
		return false;
	}

	// Get the sender ID

	JsonString jsSenderID;

	if (!ReadString(jsSenderObj, "id", &jsSenderID)) {
		return false;
	}

	// Get the sender name

	JsonString jsSenderName;

	if (!ReadString(jsSenderObj, "name", &jsSenderName)) {
		return false;
	}

	// Get the message ID

	JsonString jsMessageID;

	if (!ReadString(jsObj, "id", &jsMessageID)) {
		return false;
	}

	// Get the message content

	JsonString jsMessageContent;

	if (!ReadString(jsObj, "content", &jsMessageContent)) {
		return false;
	}

	// Construct the message

	*out_message = ChatMessage(
		jsMessageID,
		jsMessageContent,
		ChatUser(jsSenderID, jsSenderName)
	);

	return true;
}

/***************************************
* DataPacket
 ***************************************/

std::string DataPacket::Encode() {

	// Create the JSON object

	JsonObject jsObj = json_create_object();

	switch (type) {
		case DataPacketType::Message: {

			// Fill the JSON object with message data

			json_object_add(&jsObj, "content", json_create_string_value(data.message.content.c_str()));

			break;
		}
	}

	// Encode the JSON object

	JsonHandler handler = soft_create_handler();
	char* serial = soft_dump_string(&handler, json_create_object_value(jsObj));

	// Cleanup

	json_object_free(&jsObj, FALSE);

	return serial;
}

DataPacket DataPacket::CreateMessagePacket(ChatMessage message) {

	DataPacket packet;

	packet.type = DataPacketType::Message;
	packet.data.message = message;

	return packet;
}

bool DataPacket::FromJSONData(DataPacketType type, std::string data, DataPacket* out_packet) {

	*out_packet = DataPacket();

	// Decode the JSON data

	JsonHandler handler = soft_create_handler();
	JsonValue jsVal = soft_load_string(&handler, (char*)data.c_str());

	if (handler.error.exists) {
		return false;
	}

	if (jsVal.type != JSON_DATA_TYPE_OBJECT) {
		// Data packet is not the correct data type

		return false;
	}

	JsonObject jsObj = jsVal.data._object;

	// Decode the data into the data packet

	switch (type) {
		case DataPacketType::Message: {

			// Parse the message object

			if (!ParseMessageObject(jsObj, &out_packet->data.message)) {
				return false;
			}

			break;
		}
		case DataPacketType::MessageList: {

			// Get the message list object

			JsonList jsMessageList;

			if (!ReadList(jsObj, "messages", &jsMessageList)) {
				return false;
			}

			// Read the messages into a message vector

			std::vector<ChatMessage> message_list;

			for (uint64 i = 0; i < jsMessageList.length; i++) {

				// Get the message JSON object

				JsonValue* jsMessageVal = json_list_get(&jsMessageList, i);

				if (jsMessageVal->type != JSON_DATA_TYPE_OBJECT) {
					return false;
				}

				JsonObject jsMessageObj = jsMessageVal->data._object;

				// Parse the message object

				ChatMessage message;

				if (!ParseMessageObject(jsMessageObj, &message)) {
					return false;
				}

				message_list.push_back(message);
			}

			out_packet->data.message_list = message_list;

			break;
		}
	}

	return true;
}
