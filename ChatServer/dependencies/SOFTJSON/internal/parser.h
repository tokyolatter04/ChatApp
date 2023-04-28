
#pragma once

#include "token.h"
#include "objects.h"
#include "error.h"

typedef struct _Parser {
	TokenList tokens;
	Token* token;
	uint64 index;
	Error error;
} Parser;

// Create a new Parser

Parser parser_init(TokenList tokens);

// Build a json value using a Parser

JsonValue parser_get_result(Parser* parser);
