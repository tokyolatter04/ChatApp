
#pragma once

#include "internal/datatypes.h"
#include "internal/objects.h"
#include "internal/error.h"
#include "internal/globals.h"
#include "internal/io.h"

#define soft_set_engine_mode(mode)			\
	json_mode = mode;

typedef struct _JsonHandler {
	Error error;
} JsonHandler;

// Create new JsonHandler

JsonHandler soft_create_handler();

// Parse a JsonValue from a source string

JsonValue soft_load_string(JsonHandler* handler, char* source);

// Parse a JsonValue from a filepath

JsonValue soft_load_file(JsonHandler* handler, char* filepath);

// Dump a JsonValue to a string

char* soft_dump_string(JsonHandler* handler, JsonValue value);

// Dump a JsonValue to a filepath

void soft_dump_file(JsonHandler* handler, JsonValue value, char* filepath);
