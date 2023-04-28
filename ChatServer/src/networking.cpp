
#include <WS2tcpip.h>

#include "../include/networking.hpp"
#include "../include/cryptography.hpp"

/***************************************
* TcpClient
 ***************************************/

 /*
	 How It Works:

	 - Client generates an RSA key pair
	 - Client sends the RSA public key to the server
	 - Server generates an AES key
	 - Server encrypts the AES key with the RSA public key and sends it to the client
	 - The client decrypts the AES key with their RSA private key
	 - AES key is now used to encrypt all traffic
 */

bool TcpClient::InitEncrytion() {

	// Receive the client's public RSA key

	char buffer[4096];
	memset(buffer, 0, 4096);
	int32 recv_len;

	if (!Receive(buffer, 4096, &recv_len)) {
		return false;
	}

	std::string public_key = std::string(buffer, recv_len);

	// Generate AES key

	std::string aes_key;

	if (!Cryptography::Aes::GenerateRandomKey(&aes_key)) {
		return false;
	}

	// Encrypt AES key with public RSA key and send it to the client

	std::string encrypted = Cryptography::Rsa::EncryptWithKey(aes_key, public_key);

	if (!Send((char*)encrypted.c_str(), encrypted.length())) {
		return false;
	}

	return false;
}

/*
	Disconnect the client if it is currently connected
	Shutdown and close the client socket
*/

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

/*
	Upload raw data to the client
	Use a thread lock for thread safe uploading that is synchronized
	Do not attempt to send if the client is already disconnected
	Set connected to false if the connection has disconnected while sending
	Return false if any errors occurred while sending
*/

bool TcpClient::Send(char* data, int32 data_len) {
	// Lock access for other threads
	// Lock is automatically unlocked when it goes out of scope

	std::lock_guard<std::mutex> lock(send_lock);

	// Exit if the client is not connected

	if (!connected) {
		return false;
	}

	// Send data to the client

	int32 result = send(sock, data, data_len, 0);

	// Detect if the client has disconnected

	if (result == SOCKET_ERROR) {
		int32 error = WSAGetLastError();

		if (error == WSAECONNRESET || error == WSAECONNABORTED || error == WSAENETRESET) {
			connected = false;
		}

		return false;
	}

	return true;
}

/*
	Receive raw data from the server
	Do not attempt to receive if the client is already disconnected
	Set connected to false if the connection has disconnected while receiving
	Return false if any errors occurred while receiving
*/

bool TcpClient::Receive(char* buffer, int32 buffer_len, int32* out_len) {
	// Exit if the client is not connected

	if (!connected) {
		return false;
	}

	// Receive data from the client

	*out_len = recv(sock, buffer, buffer_len, 0);

	// Detect if the client has disconnected

	if (*out_len == SOCKET_ERROR) {
		int32 error = WSAGetLastError();

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

/*
	Setup the server socket in WinSock
	Create a server address from the IP and port and bind that to the socket for use
*/

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

/*
	Shutdown and close the socket
	Do not attempt to shutdown if the server has not been opened
*/

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

/*
	Attempt to accept a connection from a client
	Do not attempt to accept a connection if the server is not open
	Return false if any errors occurred while accepting the connection
	Fill the 'out_client' variable with a TcpClient object
*/

bool TcpServer::Accept(TcpClient* out_client) {
	// Exit if the server has been shutdown

	if (!open) {
		return false;
	}

	// Accept connection

	SOCKET client_sock = accept(sock, (SOCKADDR*)&addr, nullptr);

	// Detect an error

	if (client_sock == INVALID_SOCKET) {
		int32 error = WSAGetLastError();

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
