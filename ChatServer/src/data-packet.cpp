
#include "../include/data-packet.hpp"

/***************************************
* Internal Helpers
 ***************************************/

 /*
	 Get a JSON object from a JSON object by its key
 */

bool ReadObject(JsonObject obj, std::string key, JsonObject* out_obj) {

	JsonValue* jsVal;

	if (!json_object_get_value(&obj, key.c_str(), &jsVal)) {
		// Failed to get the object

		return false;
	}

	if (jsVal->type != JSON_DATA_TYPE_OBJECT) {
		// Object is not the correct data type

		return false;
	}

	*out_obj = jsVal->data._object;

	return true;
}

/*
	Get a JSON list from a JSON object by its key
*/

bool ReadList(JsonObject obj, std::string key, JsonList* out_list) {

	JsonValue* jsVal;

	if (!json_object_get_value(&obj, key.c_str(), &jsVal)) {
		// Failed to get the list

		return false;
	}

	if (jsVal->type != JSON_DATA_TYPE_LIST) {
		// List is not the correct data type

		return false;
	}

	*out_list = jsVal->data._list;

	return true;
}

/*
	Get a JSON string from a JSON object by its key
*/

bool ReadString(JsonObject obj, std::string key, JsonString* out_str) {

	JsonValue* jsVal;

	if (!json_object_get_value(&obj, key.c_str(), &jsVal)) {
		// Failed to get the object

		return false;
	}

	if (jsVal->type != JSON_DATA_TYPE_STRING) {
		// Object is not the correct data type

		return false;
	}

	*out_str = jsVal->data._string;

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

			// Create the sender object

			JsonObject jsSenderObj = json_create_object();
			json_object_add(&jsSenderObj, "id", json_create_string_value(data.message.sender.id.c_str()));;
			json_object_add(&jsSenderObj, "name", json_create_string_value(data.message.sender.name.c_str()));

			// Fill the JSON object with message data

			json_object_add(&jsObj, "id", json_create_string_value(data.message.id.c_str()));
			json_object_add(&jsObj, "content", json_create_string_value(data.message.content.c_str()));
			json_object_add(&jsObj, "sender", json_create_object_value(jsSenderObj));

			break;
		}
		case DataPacketType::MessageList: {

			// Create the message list

			JsonList jsMessageList = json_create_list();

			for (ChatMessage& message : data.message_list) {

				JsonObject jsMessageObj = json_create_object();

				// Create the sender object

				JsonObject jsSenderObj = json_create_object();
				json_object_add(&jsSenderObj, "id", json_create_string_value(message.sender.id.c_str()));;
				json_object_add(&jsSenderObj, "name", json_create_string_value(message.sender.name.c_str()));

				// Fill the JSON object with message data

				json_object_add(&jsMessageObj, "id", json_create_string_value(message.id.c_str()));
				json_object_add(&jsMessageObj, "content", json_create_string_value(message.content.c_str()));
				json_object_add(&jsMessageObj, "sender", json_create_object_value(jsSenderObj));

				// Add message to the message list

				json_list_add(&jsMessageList, json_create_object_value(jsMessageObj));
			}

			// Fill the JSON object with the message list data

			json_object_add(&jsObj, "messages", json_create_list_value(jsMessageList));

			break;
		}
		case DataPacketType::UserList: {

			// Create the user list

			JsonList jsUserList = json_create_list();

			for (ChatUser& user : data.user_list) {

				// Create the user object

				JsonObject jsUserObj = json_create_object();

				json_object_add(&jsUserObj, "id", json_create_string_value(user.id.c_str()));
				json_object_add(&jsUserObj, "name", json_create_string_value(user.name.c_str()));

				// Add user to the user list

				json_list_add(&jsUserList, json_create_object_value(jsUserObj));
			}

			// Fill the JSON object with the user list data

			json_object_add(&jsObj, "users", json_create_list_value(jsUserList));

			break;
		}
	}

	// Encode the JSON object

	JsonHandler handler = soft_create_handler();
	char* serial = soft_dump_string(&handler, json_create_object_value(jsObj));

	// Cleanup

	json_object_free(&jsObj, FALSE);

	return serial;
};

DataPacket DataPacket::CreateMessagePacket(ChatMessage message) {

	DataPacket packet;

	packet.type = DataPacketType::Message;
	packet.data.message = message;

	return packet;
}

DataPacket DataPacket::CreateMessageListPacket(std::vector<ChatMessage> messages) {

	DataPacket packet;

	packet.type = DataPacketType::MessageList;
	packet.data.message_list = messages;

	return packet;
}

DataPacket DataPacket::CreateUserListPacket(std::vector<ChatUser> users) {

	DataPacket packet;

	packet.type = DataPacketType::UserList;
	packet.data.user_list = users;

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

			// Get the message content

			JsonString jsMessageContent;

			if (!ReadString(jsObj, "content", &jsMessageContent)) {
				return false;
			}

			// Construct the message

			out_packet->data.message = ChatMessage(jsMessageContent);

			break;
		}
	}

	return true;
}
