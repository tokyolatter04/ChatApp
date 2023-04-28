
#pragma once

#include "lists.h"
#include "error.h"

typedef struct _Lexer {
	char* source;
	uint64 length;
	char character;
	uint64 index;
	Error error;
} Lexer;

// Create a new Lexer

Lexer lexer_init(char* source);

// Build a token list using a Lexer

TokenList lexer_get_result(Lexer* lexer);
