
#include <iostream>
#include <WS2tcpip.h>

#include "../include/networking.hpp"
#include "../include/cryptography.hpp"

/*
	Setup the client's socket in WinSock
	Create a server address from the IP and port
*/

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

	server_addr = { 0 };

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);

	if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) == SOCKET_ERROR) {
		// Invalid or unsupported IP address

		return false;
	}

	return true;
}

/*
	Connect to the server
	Keep retrying until the connection works
	Sleep for 2 seconds between connections
*/

void TcpClient::Connect() {
	while (true) {
		// Attempt to connect to the server

		int32 result = connect(sock, (SOCKADDR*)&server_addr, sizeof(SOCKADDR_IN));

		if (result != SOCKET_ERROR) {
			// Connection attempt was accepted

			connected = true;
			break;
		}

		// Connection attempt rejected, retrying in 2 seconds...

		Sleep(2000);
	}
}

/*
	Disconnect from the server if it is currently connected
	Shutdown and close the sockets
*/

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

/*
	How It Works:

	- Client generates an RSA key pair
	- Client sends the RSA public key to the server
	- Server generates an AES key
	- Server encrypts the AES key with the RSA public key and sends it to the client
	- The client decrypts the AES key with their RSA private key
	- AES key is now used to encrypt all traffic
*/

bool TcpClient::InitEncryption() {

	// Generate and sent an RSA public key

	Cryptography::Rsa rsa;
	std::string public_key = rsa.GetPublicKey();

	if (!Send((char*)public_key.c_str(), public_key.length())) {
		return false;
	}

	// Receive the encrypted AES key

	char buffer[4096];
	memset(buffer, 0, 4096);
	int32 recv_len;

	if (!Receive(buffer, 4096, &recv_len)) {
		return false;
	}

	std::string encrypted_aes_key = std::string(buffer, recv_len);

	// Decrypt the AES key with the RSA private key

	std::string aes_key = rsa.Decrypt(encrypted_aes_key);

	// Create protocol instance with AES key

	protocol = Protocol(aes_key);

	return true;
}

/*
	Upload a packet using the custom Protocol
	Return false if any errors occurred while sending
*/

bool TcpClient::SendPacket(std::string channel, std::string data, std::vector<std::string> flags) {
	// Build packet

	std::string packet = protocol.BuildPacket(channel, data, flags);

	// Send packet

	if (!Send((char*)packet.c_str(), packet.length())) {
		return false;
	}

	return true;
}

/*
	Upload raw data from the server
	Use a thread lock for thread safe uploading that is synchronized
	Do not attempt to send if the client is already disconnected
	Set connected to false if the connection has disconnected while sending
	Return false if any errors occurred while sending
*/

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

/*
	Receive raw data from the server
	Do not attempt to receive if the client is already disconnected
	Set connected to false if the connection has disconnected while receiving
	Return false if any errors occurred while receiving
*/

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

/*
	Begin reading data from the client
	Push this data into the packet decoder
	Stop reading when the client has disconnected
*/

void TcpClient::StartListening() {

	while (IsConnected()) {
		// Receive data from client

		static char data[4096];
		memset(data, 0, 4096);
		int32 len_recv = 0;

		if (!Receive(data, 4096, &len_recv)) {
			// Error receiving data

			continue;
		}

		// Push packet into protocol

		protocol.PushBytes(std::string(data, len_recv));
	}
}

/*
	Dequeue a packet from the packet queue
	Return false if there is nothing to dequeue
*/

bool TcpClient::GetPacket(RawPacket* out_packet) {
	return protocol.GetPacket(out_packet);
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

