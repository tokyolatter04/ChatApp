
#pragma once

// Char values of syntax tokens

#define CHAR_COMMA							','
#define CHAR_COLON							':'
#define CHAR_QUOTE							'"'
#define CHAR_LBRACKET						'['
#define CHAR_RBRACKET						']'
#define CHAR_LCURLY							'{'
#define CHAR_RCURLY							'}'
#define CHAR_EOF							'\0'

// String values of keyword tokens

#define KEYWORD_TRUE						"true"
#define KEYWORD_FALSE						"false"
#define KEYWORD_NULL						"null"

typedef enum _TokenType {
	// Syntax Tokens

	TOKEN_TYPE_COMMA,
	TOKEN_TYPE_COLON,
	TOKEN_TYPE_QUOTE,
	TOKEN_TYPE_LCURLY,
	TOKEN_TYPE_RCURLY,
	TOKEN_TYPE_LBRACKET,
	TOKEN_TYPE_RBRACKET,
	TOKEN_TYPE_EOF,

	// Keyword Tokens

	TOKEN_TYPE_TRUE,
	TOKEN_TYPE_FALSE,
	TOKEN_TYPE_NULL,

	// Data Tokens

	TOKEN_TYPE_NUMBER,
	TOKEN_TYPE_STRING
} TokenType;

typedef struct _Token {
	TokenType type;
	char* value;
} Token;

// Create a new Token

Token token_init(TokenType type, char* value);

// Get the string representation of a TokenType (for debugging)

char* token_type_to_string(TokenType type);
