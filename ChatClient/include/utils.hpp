
#pragma once

#include <string>

#include "types.hpp"

static class Utils {
public:
    static bool TryParseInt32(std::string str, int32* out_int);
    static bool TryParseUInt64(std::string str, uint64* out_int);

    static std::string RandomUUID();
};
