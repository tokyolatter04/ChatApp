
#pragma once

#define JSON_MODE_PRIORITY_DEFAULT			JSON_MODE_PRIORITY_PROCESSOR

/*
	JSON_MODE_PRIORITY_PROCESSOR
		Be fast if it wastes some memory
	JSON_MODE_PRIORITY_MEMORY
		Be memory efficient if it is at the cost of speed
*/

typedef enum _JsonMode {
	JSON_MODE_PRIORITY_PROCESSOR,
	JSON_MODE_PRIORITY_MEMORY
} JsonMode;

extern JsonMode json_mode;
