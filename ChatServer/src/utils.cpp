
#include <rpc.h>
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

std::string Utils::RandomUUID() {
    UUID uuid;
    UuidCreate(&uuid);

    char* uuid_str;
    UuidToStringA(&uuid, (RPC_CSTR*)&uuid_str);

    std::string result = std::string(uuid_str);
    RpcStringFreeA((RPC_CSTR*)&uuid_str);

    return result;
}
