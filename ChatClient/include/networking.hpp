
#pragma once

#include <string>
#include <mutex>
#include <Windows.h>

#include "protocol.hpp"

class TcpClient {
private:
	std::string server_ip;
	int32 server_port;

	bool connected;
	SOCKET sock;
	SOCKADDR_IN server_addr;

	Protocol protocol;
	std::mutex send_lock;
public:
	TcpClient(std::string _server_ip, int32 _server_port)
		: server_ip(_server_ip), server_port(_server_port), connected(false) {}

	TcpClient()
		: connected(false) {}

	TcpClient(const TcpClient& other)
		: server_ip(other.server_ip), server_port(other.server_port),
		connected(other.connected), sock(other.sock), server_addr(other.server_addr),
		protocol(other.protocol), send_lock() {}

	TcpClient& operator=(const TcpClient& other) {
		if (this != &other) {
			server_ip = other.server_ip;
			server_port = other.server_port;
			connected = other.connected;
			server_addr = other.server_addr;
			protocol = other.protocol;
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

	// Send a protocol packet
	bool SendPacket(std::string channel, std::string data, std::vector<std::string> flags = std::vector<std::string>());

	// Send raw data to the server
	bool Send(char* data, int32 data_len);

	// Receive raw data from the server
	bool Receive(char* buffer, int32 buffer_len, int32* out_len);

	// Start listening for data
	void StartListening();

	// Get a packet from the packet queue
	bool GetPacket(RawPacket* out_packet);
};

// Initialise WinSock
void WinSockInit();

// Uninitialise WinSock
void WinSockShutdown();
