
#pragma once

#include <string>
#include <mutex>
#include <Windows.h>

#include "protocol.hpp"

class TcpClient {
private:
	bool connected;
	SOCKET sock;

	Protocol protocol;
	std::mutex send_lock;
public:
	TcpClient(SOCKET _sock)
		: connected(true), sock(_sock) {}

	TcpClient()
		: connected(false), sock(INVALID_SOCKET) {}

	TcpClient(const TcpClient& other)
		: connected(other.connected), sock(other.sock), protocol(other.protocol),
		  send_lock() {}

	TcpClient& operator=(const TcpClient& other) {
		if (this != &other) {
			connected = other.connected;
			protocol = other.protocol;
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

	// Send a protocol packet
	bool SendPacket(std::string channel, std::string data, std::vector<std::string> flags = std::vector<std::string>());

	// Send raw data to the client
	bool Send(char* data, int32 data_len);

	// Receive raw data from the client
	bool Receive(char* buffer, int32 buffer_len, int32* out_len);

	// Start listening for data
	void StartListening();

	// Get a packet from the packet queue
	bool GetPacket(RawPacket* out_packet);
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
