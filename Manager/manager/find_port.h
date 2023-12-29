#include <WS2tcpip.h>

namespace manager {
	// Finds a free port for a new game server
	inline int find_port() {
		// Initialize Winsock
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			return -1;
		}

		// Create a socket
		SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET) {
			WSACleanup();
			return -1;
		}

		// Bind the socket to a random port
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = 0; // Let the OS choose a port
		if (bind(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
			closesocket(sock);
			WSACleanup();
			return -1;
		}

		// Get the port number
		int len = sizeof(addr);
		if (getsockname(sock, (sockaddr*)&addr, &len) == SOCKET_ERROR) {
			closesocket(sock);
			WSACleanup();
			return -1;
		}

		int port = ntohs(addr.sin_port);

		// Close the socket and clean up Winsock
		closesocket(sock);
		WSACleanup();

		// Return the port
		return port;
	}
}