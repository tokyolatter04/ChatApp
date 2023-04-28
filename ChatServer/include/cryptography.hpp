
#pragma once

#include <Windows.h>

#include <AES/aes256.hpp>

#include "types.hpp"

namespace Cryptography {
	class Aes {
	private:
		std::vector<byte> key;
		Aes256 crypt;
	public:
		Aes(std::string _key);
		Aes() {}

		static bool GenerateRandomKey(std::string* key);

		std::string Encrypt(std::string text);
		std::string Decrypt(std::string text);
	};

	class Rsa {
	private:
		HCRYPTPROV hCryptProv;
		HCRYPTKEY hCryptKey;
		HCRYPTKEY hCryptUserKey;
	public:
		Rsa();

		static std::string EncryptWithKey(std::string text, std::string publicKey);
		std::string Encrypt(std::string text);
		std::string Decrypt(std::string text);

		std::string GetPublicKey() const;
	};

	static class Sha256 {
	public:
		static std::string ComputeTextHash(std::string text);
	};
}
