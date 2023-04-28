
#include <iostream>

#include <PICOSHA2/picosha2.h>
#include <BASE64/base64.h>

#include "../include/cryptography.hpp"

namespace Cryptography {

	////////////////////////////
	/////////// AES ////////////
	////////////////////////////

#define StringToBytes(string)   \
    std::vector<byte>(string.c_str(), string.c_str() + string.length())

#define BytesToString(bytes)    \
    std::string(bytes.begin(), bytes.end())

	Aes::Aes(std::string _key) {
		key = StringToBytes(_key);
		crypt = Aes256(key);
	}

	bool Aes::GenerateRandomKey(std::string* key) {

		HCRYPTPROV hCryptProv;
		byte buffer[32];

		// Acquire API instance

		if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
			return false;
		}

		// Generate random key

		if (!CryptGenRandom(hCryptProv, 32, buffer)) {
			return false;
		}

		// Convert key to string

		*key = std::string((char*)buffer, 32);

		return true;
	}

	std::string Aes::Encrypt(std::string text) {

		std::vector<byte> encrypted;
		crypt.encrypt(key, StringToBytes(text), encrypted);

		return BytesToString(encrypted);
	}

	std::string Aes::Decrypt(std::string text) {

		std::vector<byte> decrypted;
		crypt.decrypt(key, StringToBytes(text), decrypted);

		return BytesToString(decrypted);
	}

	////////////////////////////
	/////////// RSA ////////////
	////////////////////////////

	Rsa::Rsa() {
		// Acquire API context

		if (!CryptAcquireContext(&hCryptProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, 0)) {
			std::cout << "CryptAcquireContext Error: " << GetLastError() << '\n';
			exit(EXIT_FAILURE);
		}

		// Generate key

		if (!CryptGenKey(hCryptProv, CALG_RC4, CRYPT_EXPORTABLE, &hCryptKey)) {
			std::cout << "CryptGenKey Error: " << GetLastError() << '\n';
			exit(EXIT_FAILURE);
		}

		// Get the user key

		if (!CryptGetUserKey(hCryptProv, AT_KEYEXCHANGE, &hCryptUserKey)) {
			std::cout << "CryptGetUserKey Error: " << GetLastError() << '\n';
			exit(EXIT_FAILURE);
		}
	}

	std::string Rsa::EncryptWithKey(std::string text, std::string publicKey) {

		publicKey = base64_decode(publicKey);

		HCRYPTPROV hProv;
		HCRYPTKEY hKey;

		// Acquire API context

		if (!CryptAcquireContext(&hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, 0)) {
			std::cout << "CryptAcquireContext Error: " << GetLastError() << '\n';
			exit(EXIT_FAILURE);
		}

		// Import key

		if (!CryptImportKey(hProv, (byte*)publicKey.data(), publicKey.length(), 0, 0, &hKey)) {
			std::cout << "CryptImportKey Error: " << GetLastError() << '\n';
			exit(EXIT_FAILURE);
		}

		// Encrypt text

		uint64 length = text.length();

		std::string encrypted;
		encrypted.assign(text);

		if (!CryptEncrypt(hKey, 0, TRUE, 0, (byte*)encrypted.data(), &length, length)) {
			std::cout << "CryptEncrypt Error: " << GetLastError() << '\n';
			exit(EXIT_FAILURE);
		}

		return encrypted;
	}

	std::string Rsa::Encrypt(std::string text) {

		uint64 length = text.length();

		std::string encrypted;
		encrypted.assign(text);

		if (!CryptEncrypt(hCryptKey, 0, TRUE, 0, (byte*)encrypted.data(), &length, length)) {
			std::cout << "CryptEncrypt Error: " << GetLastError() << '\n';
			exit(EXIT_FAILURE);
		}

		return encrypted;
	}

	std::string Rsa::Decrypt(std::string text) {

		uint64 length = text.length();

		std::string decrypted;
		decrypted.assign(text);

		if (!CryptDecrypt(hCryptKey, 0, TRUE, 0, (byte*)decrypted.data(), &length)) {
			std::cout << "CryptDecrypt Error: " << GetLastError() << '\n';
			exit(EXIT_FAILURE);
		}

		return decrypted;
	}

	std::string Rsa::GetPublicKey() const {
		// Get the public key length

		DWORD dwPublicKeyLen;

		if (!CryptExportKey(hCryptKey, hCryptUserKey, SIMPLEBLOB, 0, NULL, &dwPublicKeyLen)) {
			std::cout << "CryptExportKey Error: " << GetLastError() << '\n';
			exit(EXIT_FAILURE);
		}

		// Get the public key

		byte* publicKey = new byte[dwPublicKeyLen];

		if (!CryptExportKey(hCryptKey, hCryptUserKey, SIMPLEBLOB, 0, publicKey, &dwPublicKeyLen)) {
			std::cout << "CryptExportKey Error: " << GetLastError() << '\n';
			exit(EXIT_FAILURE);
		}

		// Encode key in base64

		return base64_encode(std::string((char*)publicKey, dwPublicKeyLen));
	}

	///////////////////////////////
	/////////// SHA256 ////////////
	///////////////////////////////

	std::string Sha256::ComputeTextHash(std::string text) {
		// Compute hash

		std::vector<byte> hash(picosha2::k_digest_size);
		picosha2::hash256(text.begin(), text.end(), hash.begin(), hash.end());

		// Encode to hex

		return picosha2::bytes_to_hex_string(hash.begin(), hash.end());
	}
}
