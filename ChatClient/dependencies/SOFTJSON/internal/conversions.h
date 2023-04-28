
#pragma once

#include "datatypes.h"

typedef struct _Number {
	enum {
		NUMBER_TYPE_INT,
		NUMBER_TYPE_FLOAT
	} type;

	union {
		int64 _int;
		float64 _float;
	} data;
} Number;

uint8 string_to_number(char* string, Number* number);
