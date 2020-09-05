// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <conio.h>
#ifdef _WIN32  
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#endif

#ifdef _WIN32
#define ISVALIDSCOKET(s)( (s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define ISVALIDSCOKET(s)((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif
auto constexpr port = "80";
auto constexpr hostname = "example.com";

int main()
{
#ifdef _WIN32
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		std::cerr << "Failed WSAStartup";
		return -1;
	}
#endif
	struct addrinfo hints;
	struct addrinfo* peer_addr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(hostname, port, &hints, &peer_addr))
	{
		std::cerr << "Getaddrinfo....Failed";
		return -1;
	}
	std::cout << "Remote addresss printing .....";
	char address_buff[100];
	char service_buff[100];
	getnameinfo(peer_addr->ai_addr, peer_addr->ai_addrlen, address_buff, sizeof(address_buff), service_buff, sizeof(service_buff), NI_NUMERICHOST);
	printf("\n _____________________________________________________\n");
	printf("|                                       |             |\n");
	printf("| Address_buff                          | Service_buff|");
	printf("\n|---------------------------------------|-------------|");
	printf("\n| %s    | %s        |", address_buff, service_buff);
	printf("\n|_______________________________________|_____________|\n");
	printf("\ncreating Socket");
	SOCKET socket_peer;
	socket_peer = socket(peer_addr->ai_family, peer_addr->ai_socktype, peer_addr->ai_protocol);
	if (!ISVALIDSCOKET(socket_peer))
	{
		fprintf(stderr, "\nsocet() failed. :\t(%d)\n", GETSOCKETERRNO());
		return 1;
	}

	printf("\nConnecting...\n");
	if (connect(socket_peer, peer_addr->ai_addr, peer_addr->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	freeaddrinfo(peer_addr);
	printf("Connected to the server ");
	while (true)
	{
		fd_set reads;
		FD_ZERO(&reads);
		FD_SET(socket_peer, &reads);
#ifndef _WIN32
		FD_SET(0, &reads);
#endif
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;
		auto val = select(socket_peer + 1, &reads, 0, 0, &timeout);
		if (val < 0)
		{
			std::cerr << "\nSelect failed : " << GETSOCKETERRNO() << std::endl;
			return 1;
		}
		if (FD_ISSET(socket_peer, &reads))
		{
			char read[4096];
			int bytes_received = recv(socket_peer, read, 4096, 0);
			if (bytes_received < 1)
			{
				std::cout << "Connection is closed by peers" << std::endl;
				break;
			}

			printf("Recieved (%d bytes): %.*s", bytes_received, bytes_received, read);
		}

#ifdef _WIN32
		if (_kbhit())
		{
#else
		if (FD_ISSET(0, &reads))
		{
#endif
			char read[4096];
			if (!fgets(read, 4096, stdin))
				break;
			printf("Sending : %s", read);
			int bytes_sent = send(socket_peer, read, strlen(read), 0);
			printf("sent %d bytes", bytes_sent);
		}
		}


	printf("Closing Socket");
	CLOSESOCKET(socket_peer);
#ifdef _WIN32
	WSACleanup();
#endif
	printf("Finished\n");
	return 0;


}