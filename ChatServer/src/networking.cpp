
#include <WS2tcpip.h>

#include "../include/networking.hpp"

/***************************************
* TcpClient
 ***************************************/

void TcpClient::Disconnect() {
	// Exit if the client is already disconnected

	if (!connected) {
		return;
	}

	connected = false;

	// Close socket

	shutdown(sock, SD_BOTH);
	closesocket(sock);
}

bool TcpClient::IsConnected() const {
	return connected;
}

bool TcpClient::Send(char* data, int data_len) {
	// Lock access for other threads
	// Lock is automatically unlocked when it goes out of scope

	std::lock_guard<std::mutex> lock(send_lock);

	// Exit if the client is not connected

	if (!connected) {
		return false;
	}

	// Send data to the client

	int result = send(sock, data, data_len, 0);

	// Detect if the client has disconnected

	if (result == SOCKET_ERROR) {
		int error = WSAGetLastError();

		if (error == WSAECONNRESET || error == WSAECONNABORTED || error == WSAENETRESET) {
			connected = false;
		}

		return false;
	}

	return true;
}

bool TcpClient::Receive(char* buffer, int buffer_len, int* out_len) {
	// Exit if the client is not connected

	if (!connected) {
		return false;
	}

	// Receive data from the client

	*out_len = recv(sock, buffer, buffer_len, 0);

	// Detect if the client has disconnected

	if (*out_len == SOCKET_ERROR) {
		int error = WSAGetLastError();

		if (error == WSAECONNRESET || error == WSAECONNABORTED || error == WSAENETRESET) {
			connected = false;
		}

		return false;
	}
	else if (*out_len == 0) {
		connected = false;

		return false;
	}

	return true;
}

 /***************************************
 * TcpServer
  ***************************************/

bool TcpServer::Open() {
	open = true;

	// Create tcp socket

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET) {
		// Socket error

		return false;
	}

	// Bind socket

	addr = { 0 };

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
		// Bind error

		return false;
	}

	// Listen on the socket

	listen(sock, 2);

	return true;
}

bool TcpServer::Shutdown() {
	// Exit if the server has already been shutdown

	if (!open) {
		return false;
	}

	open = false;

	// Close server socket

	closesocket(sock);

	return true;
}

bool TcpServer::IsOpen() const {
	return open;
}

bool TcpServer::Accept(TcpClient* out_client) {
	// Exit if the server has been shutdown

	if (!open) {
		return false;
	}

	// Accept connection

	SOCKET client_sock = accept(sock, (SOCKADDR*)&addr, nullptr);

	// Detect an error

	if (client_sock == INVALID_SOCKET) {
		int error = WSAGetLastError();

		if (error == WSAEINTR || error == WSAEWOULDBLOCK) {
			return false;
		}
	}

	// Create Tcp Client

	*out_client = TcpClient(client_sock);

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
