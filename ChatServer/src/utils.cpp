
#include <rpc.h>
#include <random>
#include <sstream>
#include <Windows.h>

#include "../include/utils.hpp"

bool Utils::TryParseInt32(std::string str, int32* out_int) {
    try {
        *out_int = std::stoi(str);

        return true;
    }
    catch (...) {
        return false;
    }
}

bool Utils::TryParseUInt64(std::string str, uint64* out_int) {
    try {
        *out_int = std::stoull(str);

        return true;
    }
    catch (...) {
        return false;
    }
}

std::string Utils::RandomDigits(uint64 size) {

    // Setup random number generator

    const std::string CHARSET = "0123456789";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, CHARSET.length() - 1);

    // Generate the random string

    std::stringstream digits;

    for (uint64 i = 0; i < size; i++) {
        digits << CHARSET[dis(gen)];
    }

    return digits.str();
}

std::string Utils::RandomUUID() {
    UUID uuid;
    UuidCreate(&uuid);

    char* uuid_str;
    UuidToStringA(&uuid, (RPC_CSTR*)&uuid_str);

    std::string result = std::string(uuid_str);
    RpcStringFreeA((RPC_CSTR*)&uuid_str);

    return result;
}
