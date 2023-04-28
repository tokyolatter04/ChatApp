
#pragma once

#include "objects.h"

char* serialise_json_object(JsonObject object);
char* serialise_json_list(JsonList list);
char* serialise_keyword(JsonType keyword_type);
char* serialise_json_int(JsonInt _int);
char* serialise_json_float(JsonFloat _float);
char* serialise_json_string(JsonString string);
char* serialise_json_value(JsonValue value);
char* serialise_json_pair(JsonPair pair);
