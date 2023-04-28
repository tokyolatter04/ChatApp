
#pragma once

#include "lists.h"

////////////////////////////////////////
/////////// JSON DATA TYPES ////////////
////////////////////////////////////////

typedef JsonPairList						JsonObject;
typedef JsonValueList						JsonList;
typedef int64								JsonInt;
typedef float64								JsonFloat;
typedef const char*							JsonString;
typedef bool8								JsonBool;

// Create a new JsonObject

JsonObject json_create_object();

// Append a JsonPair to a JsonObject

void json_object_add(JsonObject* object, const char* key, struct _JsonValue value);

// Get a JsonPair by index in a JsonObject

struct _JsonPair* json_object_get(JsonObject* object, uint64 index);

// Get a JsonPair by key reference in a JsonObject

bool8 json_object_get_value(JsonObject* object, const char* key, struct _JsonValue** value);

// Create a new JsonList

JsonList json_create_list();

// Append a JsonValue to a JsonList

void json_list_add(JsonList* list, struct _JsonValue value);

// Get a JsonValue by index in a JsonList

struct _JsonValue* json_list_get(JsonList* list, uint64 index);

typedef enum _JsonType {
	// Containers

	JSON_DATA_TYPE_OBJECT,
	JSON_DATA_TYPE_LIST,

	// Key Words

	JSON_DATA_TYPE_TRUE,
	JSON_DATA_TYPE_FALSE,
	JSON_DATA_TYPE_NULL,

	// Data Types

	JSON_DATA_TYPE_INT,
	JSON_DATA_TYPE_FLOAT,
	JSON_DATA_TYPE_STRING,
	JSON_DATA_TYPE_BOOL
} JsonType;

///////////////////////////////////
/////////// JSON VALUE ////////////
///////////////////////////////////

typedef struct _JsonValue {
	JsonType type;

	union {
		JsonObject _object;
		JsonList _list;
		JsonInt _int;
		JsonFloat _float;
		JsonString _string;
	} data;
} JsonValue;

// Create a JsonValue with a type and memory buffer

JsonValue json_create_value(JsonType type, void* data);

// Create a JsonValue with a JsonObject

JsonValue json_create_object_value(JsonObject object);

// Create a JsonValue with a JsonList

JsonValue json_create_list_value(JsonList list);

// Create a JsonValue with a JsonInt

JsonValue json_create_int_value(JsonInt _int);

// Create a JsonValue with a JsonFloat

JsonValue json_create_float_value(JsonFloat _float);

// Create a JsonValue with a JsonString

JsonValue json_create_string_value(JsonString string);

// Create a JsonValue with a JsonBool

JsonValue json_create_bool_value(JsonBool _bool);

// Create a JsonVal

JsonValue json_create_null_value();

//////////////////////////////////
/////////// JSON PAIR ////////////
//////////////////////////////////

typedef struct _JsonPair {
	char* key;
	JsonValue value;
} JsonPair;

JsonPair json_create_pair(const char* key, JsonValue value);

///////////////////////////////////////
/////////// MEMORY FREEING ////////////
///////////////////////////////////////

void json_object_free(JsonObject* object, bool8 loaded);
void json_list_free(JsonList* list, bool8 load);
void json_string_free(JsonString* string, bool8 loaded);
void json_value_free(JsonValue* value, bool8 loaded);
void json_pair_free(JsonPair* pair, bool8 loaded);
