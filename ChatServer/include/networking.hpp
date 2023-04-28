
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

	bool InitEncrytion();

	void Disconnect();
	bool IsConnected() const;

	bool Send(char* data, int32 data_len);
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

	bool Open();
	bool Shutdown();
	bool IsOpen() const;

	bool Accept(TcpClient* out_client);
};

void WinSockInit();
void WinSockShutdown();
