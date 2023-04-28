
#pragma once

#include <stdio.h>

#include "datatypes.h"

#define ERROR_LOG_MAX_SIZE						128

#define ERROR_TYPE_FILE_ACCESS					"Filepath '%s' Could Not Be Accessed"
#define ERROR_TYPE_UNRECOGNISED_TOKEN			"Unrecognised Token: '%c'"
#define ERROR_TYPE_UNRECOGNISED_KEYWORD			"Unrecognised Keyword: '%s'"
#define ERROR_INVALID_ESCAPE_CHARACTER			"Invalid Escape Character: '%c'"
#define ERROR_TYPE_WRONG_TYPE					"Expected Type '%s', Got '%s'"
#define ERROR_TYPE_UNEXPECTED_TYPE				"Unexpected Type '%s'"
#define ERROR_TYPE_INVALID_NUMBER				"Invalid Number: '%s'"
#define ERROR_TYPE_CALL_STACK_OVERFLOW			"Internal Call Stack Of Size '%d' Exceeded"

// Raise a fatal error which will shut down the current operation

#define RAISE_FATAL_ERROR(error, type, ...)								\
	snprintf(error.log, ERROR_LOG_MAX_SIZE, type, ##__VA_ARGS__);		\
	error.exists = TRUE;

// Wrap a function that returns a SUCCESS or FAILURE
// Closes the caller function if the function returns FAILURE

#define ERROR_CHECK(call)												\
	if (!call)															\
		return FAILURE;

typedef struct _Error {
	char log[ERROR_LOG_MAX_SIZE];
	bool8 exists;
} Error;

// Create a new Error

Error error_init();
