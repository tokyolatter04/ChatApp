
#pragma once

#include <string>
#include <mutex>
#include <Windows.h>

#include "types.hpp"

class TcpClient {
private:
	bool connected;
	SOCKET sock;

	std::mutex send_lock;
public:
	TcpClient(SOCKET _sock)
		: connected(true), sock(_sock) {}

	TcpClient()
		: connected(false), sock(INVALID_SOCKET) {}

	TcpClient(const TcpClient& other)
		: connected(other.connected), sock(other.sock), send_lock() {}

	TcpClient& operator=(const TcpClient& other) {
		if (this != &other) {
			connected = other.connected;
			sock = other.sock;
		}

		return *this;
	}

	// Initialise the AES encryption to encrypt communication to the server
	bool InitEncrytion();

	// Disconnect the client
	void Disconnect();

	// Check if the client is or is not connected
	bool IsConnected() const;

	// Send raw data to the client
	bool Send(char* data, int32 data_len);

	// Receive raw data from the client
	bool Receive(char* buffer, int32 buffer_len, int32* out_len);
};

class TcpServer {
private:
	std::string ip;
	int32 port;

	bool open;
	SOCKET sock;
	SOCKADDR_IN addr;
public:
	TcpServer(std::string _ip, int32 _port)
		: ip(_ip), port(_port), open(false) {}

	TcpServer()
		: open(false) {}

	// Setup the TcpServer socket
	bool Open();

	// Shutdown the server
	bool Shutdown();

	// Check if the server is or is not open
	bool IsOpen() const;

	// Attempt to accept a new connection
	bool Accept(TcpClient* out_client);
};

// Initialise WinSock
void WinSockInit();

// Uninitialise WinSock
void WinSockShutdown();
