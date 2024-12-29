#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <functional>
#include <thread>
#include <random>
#include <WinSock2.h>
#include <WS2tcpip.h>
#undef pascal // Remove the keyword so we can use it
#pragma comment(lib, "ws2_32.lib")

#ifndef WIN32
#error "Pascal only works on Windows"
#endif

namespace pascal {
	// Pascal result codes. Every function returns one of these
	enum result {
		PASCAL_OK,
		PASCAL_FAILED,
		PASCAL_INIT_FAILED,
		PASCAL_PING_FAILED,
		PASCAL_CONNECTION_FAILED,
		PASCAL_SEND_FAILED,
		PASCAL_MALFORMED_PACKET
	};

	enum packet_type {
		PASCAL_PACKET_OFFLINE_PING,
		PASCAL_PACKET_PING,
		PASCAL_PACKET_CONNECT,
		PASCAL_PACKET_CONNECTION_ACCEPTED,
		PASCAL_PACKET_DISCONNECT,
		PASCAL_PACKET_DATA,
		PASCAL_PACKET_JOIN_GAME,
		PASCAL_PACKET_GAME_JOINED,
		PASCAL_PACKET_GAME_LOADED,
		PASCAL_PACKET_CREATE,
		PASCAL_PACKET_DESTROY,
		PASCAL_PACKET_REFLECT,
		PASCAL_PACKET_REMOTE_EVENT,
		PASCAL_PACKET_CHAT
	};

	// Packet structure for sending and receiving
	struct packet {
		packet_type type;
		std::string guid;
		std::string data;

		packet() {
			type = PASCAL_PACKET_PING;
			guid = "";
			data = "";
		}

		// Convert packet to string for sending
		std::string to_string() {
			std::ostringstream oss;
			oss << std::setfill('0') << std::setw(2) << static_cast<int>(type);
			oss << guid;
			oss << data;
			return oss.str();
		}

		result from_string(std::string str) {
			if (str.length() < 18) {
				return PASCAL_MALFORMED_PACKET;
			}

			// Extract the type (first two characters)
			type = static_cast<packet_type>(std::stoi(str.substr(0, 2))); // Convert string to int

			guid = str.substr(2, 16); // GUIDs are 16 characters
			data = str.substr(18); // Data is everything after the first 18 characters

			return PASCAL_OK;
		}
	};

	// Structure for a connected client used by the server
	struct peer {
		struct sockaddr_in address = { 0 };
		std::string guid = "";
		std::string ip = "0.0.0.0";
		std::chrono::time_point<std::chrono::steady_clock> latest_ping = std::chrono::steady_clock::now();
	};

	class client {
	public:
		result init() {
			if (FAILED(WSAStartup(MAKEWORD(2, 2), &wsaData))) {
				return PASCAL_INIT_FAILED;
			}

			sock = socket(AF_INET, SOCK_DGRAM, 0);

			return PASCAL_OK;
		}

		~client() {
			disconnect("Client left the game");
			closesocket(sock);
			WSACleanup();
		}

		result connect(const std::string& serverIp, int serverPort) {
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(static_cast<u_short>(serverPort));

			// Convert server IP address from string to binary form using inet_pton
			if (inet_pton(AF_INET, serverIp.c_str(), &(serverAddr.sin_addr)) != 1) {
				return PASCAL_CONNECTION_FAILED;
			}

			if (::connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
				return PASCAL_CONNECTION_FAILED;
			}

			// Begin the packet loop
			// This loop listens to any received packets and parses them
			packet_loop();

			// Request to connection to the server
			packet conn_req;
			conn_req.type = PASCAL_PACKET_CONNECT;
			// PASCAL_OK from send doesn't actually mean a successful connection!
			// Use on_connect callback to check if the connection was successful
			return send(conn_req);
		}

		// Create a callback for when the client is connected
		void on_connect(std::function<void()> callback) {
			connect_callbacks.push_back(callback);
		}

		// Create a callback for when the client is disconnected
		void on_disconnect(std::function<void(std::string)> callback) {
			disconnect_callbacks.push_back(callback);
		}

		// Create a callback for when a packet is received
		void on_packet(std::function<void(packet_type, std::string)> callback) {
			packet_callbacks.push_back(callback);
		}

		// Pings a server to check if it exists
		result ping(const std::string& serverIp, int serverPort) {
			sockaddr_in server_addr;
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(static_cast<u_short>(serverPort));

			if (inet_pton(AF_INET, serverIp.c_str(), &(server_addr.sin_addr)) != 1) {
				return PASCAL_PING_FAILED;
			}

			SOCKET pingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (pingSocket == INVALID_SOCKET) {
				return PASCAL_PING_FAILED;
			}

			// Attempt to send an offline ping (0) to the server
			int sentBytes = sendto(pingSocket, "0", static_cast<int>(strlen("0")), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

			closesocket(pingSocket);

			if (sentBytes == SOCKET_ERROR) {
				return PASCAL_PING_FAILED;
			}

			return PASCAL_OK;
		}

		// Send data to a connected server
		result send(const std::string& data, packet_type type = PASCAL_PACKET_DATA) {
			if (guid == "") {
				return PASCAL_SEND_FAILED;
			}

			packet pkt;
			pkt.type = type;
			pkt.guid = guid;
			pkt.data = data;

			std::string str = pkt.to_string();
			sendto(sock, str.c_str(), static_cast<int>(str.size()), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

			return PASCAL_OK;
		}

		// Disconnect from a connected server
		result disconnect(std::string reason = "") {
			if (guid != "") {
				packet pkt;
				pkt.type = PASCAL_PACKET_DISCONNECT;
				pkt.guid = guid;
				pkt.data = reason;

				std::string str = pkt.to_string();
				sendto(sock, str.c_str(), static_cast<int>(str.size()), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

				connected = false;

				return PASCAL_OK;
			}
			else {
				return PASCAL_FAILED;
			}
		}

		// Destroys the client
		void destroy() {
			delete this;
		}

		// Returns the client's GUID
		std::string get_guid() {
			return guid;
		}

		bool connected = false;
		bool joined = false;
	private:
		result send(packet pkt) {
			std::string str = pkt.to_string();
			sendto(sock, str.c_str(), static_cast<int>(str.size()), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

			return PASCAL_OK;
		}

		// The main loop for the client
		void packet_loop() {
			// Start a thread to receive packets
			std::thread packet_thread([this]() {
				while (true) {
					char buffer[9999];
					int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);

					// If the connection is closed, destroy the client
					if (bytesReceived == 0 || bytesReceived == SOCKET_ERROR) {
						break;
					}

					// Prevent buffer overflow
					if (bytesReceived < sizeof(buffer)) {
						// Null-terminate the received data so we know when the data ends
						buffer[bytesReceived] = '\0';
						std::string str(buffer);

						// Parse packet for handling
						packet pkt;
						pkt.from_string(str);

						// Handle connection accepts from the server
						if (pkt.type == PASCAL_PACKET_CONNECTION_ACCEPTED) {
							connected = true; // We're now connected to the server

							// Set the client's unique GUID (also in the packet's data)
							guid = pkt.guid;

							// Call all connect callbacks because we're actually connected
							for (auto& callback : connect_callbacks) {
								callback();
							}

							// Run the ping loop
							ping_loop();
						}
						// Handle disconnects from the server (forced, server issues, etc.)
						else if (pkt.type == PASCAL_PACKET_DISCONNECT) {
							for (auto& callback : disconnect_callbacks) {
								callback(pkt.data);
							}
						}
						// Passthrough all other packets
						else {
							for (auto& callback : packet_callbacks) {
								callback(pkt.type, pkt.data);
							}
						}
					}
				}
				});

			packet_thread.detach();
		}

		// Ping loop function for  sending pings to the server every second
		void ping_loop() {
			std::thread ping_thread([this]() {
				while (connected) {
					// Create the packet
					packet ping_packet;
					ping_packet.type = PASCAL_PACKET_PING;
					ping_packet.guid = guid;

					// Send the ping
					send(ping_packet);

					// Sleep for one second
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
				});

			ping_thread.detach();
		}

		std::string guid = "";
		std::vector<std::function<void()>> connect_callbacks;
		std::vector<std::function<void(std::string)>> disconnect_callbacks;
		std::vector<std::function<void(packet_type, std::string)>> packet_callbacks;
		WSADATA wsaData = { 0 };
		SOCKET sock = INVALID_SOCKET;
		struct sockaddr_in serverAddr = { 0 };
	};

	class server {
	public:
		result init() {
			if (FAILED(WSAStartup(MAKEWORD(2, 2), &wsaData))) {
				return PASCAL_INIT_FAILED;
			}

			sock = socket(AF_INET, SOCK_DGRAM, 0);

			return PASCAL_OK;
		}

		~server() {
			closesocket(sock);
			WSACleanup();
		}

		result start(int port) {
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(static_cast<u_short>(port));
			serverAddr.sin_addr.s_addr = INADDR_ANY;

			// Bind socket to port
			if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
				return PASCAL_FAILED;
			}

			// Start the server receiving and ping loop
			server_loop();
			check_client_pings();

			return PASCAL_OK;
		}

		// Create a callback for when a peer is connected
		void on_connect(std::function<void()> callback) {
			connect_callbacks.push_back(callback);
		}

		// Create a callback for when a peer is disconnected
		void on_disconnect(std::function<void(peer, std::string)> callback) {
			disconnect_callbacks.push_back(callback);
		}

		// Create a callback for when a packet is received
		void on_packet(std::function<void(peer, packet_type, std::string)> callback) {
			packet_callbacks.push_back(callback);
		}

		// Send data to a peer
		result send(peer p, std::string data, packet_type type = PASCAL_PACKET_DATA) {
			// Form the packet
			packet pkt;
			pkt.guid = p.guid;
			pkt.type = type;
			pkt.data = data;

			// Stringify and send!
			send(pkt, p);

			return PASCAL_OK;
		}

		result broadcast(std::string data, packet_type type = PASCAL_PACKET_DATA) {
			packet pkt;
			pkt.type = type;
			pkt.data = data;

			// Broadcast!
			for (auto& p : peers) {
				pkt.guid = p.guid;
				send(pkt, p);
			}

			return PASCAL_OK;
		}

		// Disconnect a client from the server with an optional reason
		result disconnect(peer p, std::string reason = "") {
			for (auto& cli : peers) {
				if (cli.guid == p.guid) {
					// Send disconnect packet
					packet pkt;
					pkt.guid = p.guid;
					pkt.type = PASCAL_PACKET_DISCONNECT;
					pkt.data = reason;
					send(pkt, p);

					// Remove peer from our list
					peers.erase(std::remove_if(peers.begin(), peers.end(),
						[&](peer _p) { return _p.guid == p.guid; }),
						peers.end());

					// Call disconnect callbacks
					for (auto& callback : disconnect_callbacks) {
						callback(p, reason);
					}

					return PASCAL_OK;
				}
			}

			return PASCAL_FAILED;
		}

		// Destroys the server
		void destroy() {
			running = false;
			delete this;
		}

		// Stores all connected clients
		std::vector<peer> peers = { };
		
		// Generates a random ID for a new client
		static std::string generate_client_id() {
			const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";

			std::string randomString(16, ' ');
			std::random_device rd;
			std::mt19937 generator(rd());
			std::uniform_int_distribution<int> distribution(0, static_cast<int>(characters.size()) - 1);

			std::generate_n(randomString.begin(), 16, [&]() { return characters[distribution(generator)]; });

			return randomString;
		}
	private:
		result send(packet pkt, peer p) {
			std::string str = pkt.to_string();
			sendto(sock, str.c_str(), static_cast<int>(str.size()), 0, (struct sockaddr*)&p.address, sizeof(p.address));

			return PASCAL_OK;
		}

		// Main server loop
		void server_loop() {
			std::thread server_loop([&]() {
				char buffer[9999];
				int addrLen = sizeof(clientAddr);

				while (running) {
					int bytesReceived = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &addrLen);

					// Prevent buffer overflow
					if (bytesReceived < sizeof(buffer)) {
						buffer[bytesReceived] = '\0';

						std::string data(buffer, bytesReceived);

						// First two characters is the ID of the packet
						switch (std::stoi(data.substr(0, 2))) {
						// Handle client ping packets
						case PASCAL_PACKET_PING:
						{
							// Parse packet for handling
							packet pkt;
							pkt.from_string(data);

							// Find the peer by GUID
							for (auto& peer : peers) {
								if (peer.guid == pkt.guid) {
									// Update the timestamp for the connected client
									// Used to determine if a client has disconnected (10 seconds of no pings)
									peer.latest_ping = std::chrono::steady_clock::now();
									break;
								}
							}
							
							break;
						}
						// Connection attempt by client			
						case PASCAL_PACKET_CONNECT:
						{
							// Generate a new ID for the client
							std::string id = generate_client_id();

							// Make connected packet. Has the ID of the client for them to use
							packet pkt;
							pkt.type = PASCAL_PACKET_CONNECTION_ACCEPTED;
							pkt.guid = id;
							pkt.data = id;
							std::string str = pkt.to_string();
							sendto(sock, str.c_str(), static_cast<int>(str.size()), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));

							// Get IP address
							std::string client_ip(INET_ADDRSTRLEN, '\0');
							inet_ntop(AF_INET, &(clientAddr.sin_addr), &client_ip[0], INET_ADDRSTRLEN);

							// Add peer to the peer list
							peer cli;
							cli.guid = id;
							cli.ip = std::string(client_ip);
							cli.address = clientAddr;
							peers.push_back(cli);

							// Call connection callbacks
							for (auto& callback : connect_callbacks) {
								callback();
							}

							break;
						}

						// Disconnect
						case PASCAL_PACKET_DISCONNECT:
						{
							// Parse packet for handling
							packet pkt;
							pkt.from_string(data);

							// Check if pkt.guid is in one of the clients (is this one of our clients?)
							for (auto p : peers) {
								if (p.guid == pkt.guid) {
									// Remove the peer from the list
									peers.erase(std::remove_if(peers.begin(), peers.end(),
										[&](peer _p) { return _p.guid == p.guid; }),
										peers.end());
									
									for (auto& callback : disconnect_callbacks) {
										callback(p, pkt.data);
									}
								}
							}

							break;
						}

						// Passthrough any messages since it's easier
						default:
						{
							// Parse packet for handling
							packet pkt;
							pkt.from_string(data);

							// Check if pkt.guid is in one of the clients (is this one of our clients?)
							for (auto& peer : peers) {
								if (peer.guid == pkt.guid) { // TODO: Add IP check (not VPN friendly)
									for (auto& callback : packet_callbacks) {
										callback(peer, pkt.type, pkt.data);
									}
									
									// We can also update their ping here
									peer.latest_ping = std::chrono::steady_clock::now();
								}
							}

							break;
						}
						}
					}

					// This sleep is for preventing high CPU usage
					Sleep(1000 / 60);
				}
				});

			server_loop.detach();
		}

		// Function to check client pings
		void check_client_pings() {
			std::thread ping_check_thread([&]() {
				while (running) {
					// Check ping of each client
					for (auto& peer : peers) {
						// Assuming peer is an instance of the peer structure
						std::chrono::time_point<std::chrono::steady_clock> curr_time = std::chrono::steady_clock::now();
						std::chrono::duration<double> elapsed = curr_time - peer.latest_ping;

						// If the client hasn't pinged for 5 seconds, disconnect them
						if (elapsed.count() > 5.0) {
							disconnect(peer, "Connection timed out, please check your internet connection.");
						}
					}

					// Loops every second
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
			});

			ping_check_thread.detach();
		}

		bool running = true;
		std::vector<std::function<void()>> connect_callbacks;
		std::vector<std::function<void(peer, std::string)>> disconnect_callbacks;
		std::vector<std::function<void(peer, packet_type, std::string)>> packet_callbacks;
		WSADATA wsaData = { 0 };
		SOCKET sock = INVALID_SOCKET;
		struct sockaddr_in serverAddr, clientAddr = { 0 };
	};
}