
#include <WS2tcpip.h>

#include "../include/networking.hpp"

bool TcpClient::Open() {
	// Create socket

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET) {
		// Socket error

		return false;
	}

	// Setup connect timeout of 5 seconds

	int32 connect_timeout = 5000;

	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&connect_timeout, sizeof(int32));

	// Create server address

	addr = { 0 };

	addr.sin_family = AF_INET;
	addr.sin_port = htons(server_port);

	if (inet_pton(AF_INET, server_ip.c_str(), &addr.sin_addr) == SOCKET_ERROR) {
		// Invalid or unsupported IP address

		return false;
	}

	return true;
}

void TcpClient::Connect() {
	while (true) {
		// Attempt to connect to the server

		int32 result = connect(sock, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));

		if (result != SOCKET_ERROR) {
			// Connection attempt was accepted

			connected = true;
			break;
		}

		// Connection attempt rejected, retrying in 2 seconds...

		Sleep(2000);
	}
}

bool TcpClient::Disconnect() {
	// Exit if the client is already disconnected

	if (!connected) {
		return false;
	}

	connected = false;

	// Close socket

	shutdown(sock, SD_BOTH);
	closesocket(sock);
}

bool TcpClient::IsConnected() const {
	return connected;
}

bool TcpClient::Send(char* data, int32 buffer_len) {
	// Lock access for other threads
	// Lock is automatically unlocked when it goes out of scope

	std::lock_guard<std::mutex> lock(send_lock);

	// Exit if the client is not connected to the server

	if (!connected) {
		return false;
	}

	// Send data to the server

	int32 result = send(sock, data, buffer_len, 0);

	// Detect if the client has been disconnected from the server

	if (result == SOCKET_ERROR) {
		int32 error = WSAGetLastError();

		if (error == WSAECONNRESET || error == WSAECONNABORTED || error == WSAENETRESET) {
			connected = false;
		}

		return false;
	}

	return true;
}

bool TcpClient::Receive(char* buffer, int32 buffer_len, int32* out_len) {
	// Exit if the client is not connected to the server

	if (!connected) {
		return false;
	}

	// Receive data from the server

	*out_len = recv(sock, buffer, buffer_len, 0);

	// Detect if the client has been disconnected from the server

	if (*out_len == SOCKET_ERROR) {
		int32 error = WSAGetLastError();

		if (error == WSAECONNRESET || error == WSAECONNABORTED || error == WSAENETRESET) {
			connected = false;
		}

		return false;
	}

	return true;
}

void WinSockInit() {
	WSADATA wsa;

	if (!WSAStartup(MAKEWORD(2, 2), &wsa)) {
		// WSA error
	}
}

void WinSockShutdown() {
	WSACleanup();
}

