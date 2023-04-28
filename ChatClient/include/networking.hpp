
#pragma once

#include <string>
#include <mutex>
#include <Windows.h>

class TcpClient {
private:
	std::string server_ip;
	int server_port;

	bool connected;
	SOCKET sock;
	SOCKADDR_IN addr;

	std::mutex send_lock;
public:
	TcpClient(std::string _server_ip, int _server_port)
		: server_ip(_server_ip), server_port(_server_port), connected(false) {}

	TcpClient()
		: connected(false) {}

	TcpClient(const TcpClient& other)
		: server_ip(other.server_ip), server_port(other.server_port),
		  connected(other.connected), sock(other.sock), addr(other.addr),
		  send_lock() {}

	TcpClient& operator=(const TcpClient& other) {
		if (this != &other) {
			connected = other.connected;
			addr = other.addr;
			sock = other.sock;
		}

		return *this;
	}

	bool Open();
	void Connect();
	bool Disconnect();
	bool IsConnected() const;

	bool Send(char* data, int data_len);
	bool Receive(char* buffer, int buffer_len, int* out_len);
};

void WinSockInit();
void WinSockShutdown();