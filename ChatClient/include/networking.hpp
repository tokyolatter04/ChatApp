
#pragma once

#include <string>
#include <mutex>
#include <Windows.h>

#include "types.hpp"

class TcpClient {
private:
	std::string server_ip;
	int32 server_port;

	bool connected;
	SOCKET sock;
	SOCKADDR_IN server_addr;

	std::mutex send_lock;
public:
	TcpClient(std::string _server_ip, int32 _server_port)
		: server_ip(_server_ip), server_port(_server_port), connected(false) {}

	TcpClient()
		: connected(false) {}

	TcpClient(const TcpClient& other)
		: server_ip(other.server_ip), server_port(other.server_port),
		  connected(other.connected), sock(other.sock), server_addr(other.server_addr),
		  send_lock() {}

	TcpClient& operator=(const TcpClient& other) {
		if (this != &other) {
			connected = other.connected;
			server_addr = other.server_addr;
			sock = other.sock;
		}

		return *this;
	}

	// Setup the TcpClient socket
	bool Open();

	// Connect to the server
	void Connect();

	// Disconnect from the server
	bool Disconnect();

	// Check if the client is or is not connected
	bool IsConnected() const;

	// Initialise the AES encryption to encrypt communication to the client
	bool InitEncryption();

	// Send raw data to the server
	bool Send(char* data, int32 data_len);

	// Receive raw data from the server
	bool Receive(char* buffer, int32 buffer_len, int32* out_len);
};

// Initialise WinSock
void WinSockInit();

// Uninitialise WinSock
void WinSockShutdown();
