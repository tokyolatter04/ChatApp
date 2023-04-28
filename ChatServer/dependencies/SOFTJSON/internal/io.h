
#pragma once

#include "datatypes.h"

// Read all text from a filepath

uint8 file_read_all_text(char* filepath, char** text);

// Write all text to a filepath

uint8 file_write_all_text(char* filepath, char* text);
