
#pragma once

#include "datatypes.h"

//////////////////////////////////
/////////// CHAR LIST ////////////
//////////////////////////////////

typedef struct _CharList {
	char* buffer;
	uint64 length;
	uint64 capacity;
} CharList;

CharList char_list_init(uint64 start_capacity);

void char_list_add(CharList* list, char element);

void char_list_remove(CharList* list, uint64 index);

///////////////////////////////////
/////////// TOKEN LIST ////////////
///////////////////////////////////

typedef struct _TokenList {
	struct _Token* buffer;
	uint64 length;
	uint64 capacity;
} TokenList;

TokenList token_list_init(uint64 start_capacity);

void token_list_add(TokenList* list, struct _Token element);

void token_list_remove(TokenList* list, uint64 index);

////////////////////////////////////////
/////////// JSON VALUE LIST ////////////
////////////////////////////////////////

typedef struct _JsonValueList {
	struct _JsonValue* buffer;
	uint64 length;
	uint64 capacity;
} JsonValueList;

JsonValueList json_value_list_init(uint64 start_capacity);

void json_value_list_add(JsonValueList* list, struct _JsonValue element);

void json_value_list_remove(JsonValueList* list, uint64 index);

///////////////////////////////////////
/////////// JSON PAIR LIST ////////////
///////////////////////////////////////

typedef struct _JsonPairList {
	struct _JsonPair* buffer;
	uint64 length;
	uint64 capacity;
} JsonPairList;

JsonPairList json_pair_list_init(uint64 start_capacity);

void json_pair_list_add(JsonPairList* list, struct _JsonPair element);

void json_pair_list_remove(JsonPairList* list, uint64 index);
